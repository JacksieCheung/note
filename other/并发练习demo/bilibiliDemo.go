package main

import (
	"encoding/json"
	"fmt"
	"io/ioutil"
	"net/http"
	"strconv"
	"time"

	"github.com/jinzhu/gorm"
	_ "github.com/jinzhu/gorm/dialects/mysql"
)

var (
	DB       *gorm.DB
	mission1 int
	mission2 int
	mission3 int
	done1    bool
	done2    bool
	done3    bool
)

type Catgory struct {
	Code    int    `json:"code"`
	Message string `json:"message"`
	TTL     int    `json:"ttl"`
	Data    struct {
		HasMore         bool   `json:"has_more"`
		Offset          string `json:"offset"`
		Total           int    `json:"total"`
		ArchiveChannels []struct {
			ID              int    `json:"id"`
			Name            string `json:"name"`
			Cover           string `json:"cover"`
			Background      string `json:"background"`
			SubscribedCount int    `json:"subscribed_count"`
			ArchiveCount    string `json:"archive_count"`
			FeaturedCount   int    `json:"featured_count"`
			ThemeColor      string `json:"theme_color"`
			Archives        []struct {
				ID         int    `json:"id"`
				Name       string `json:"name"`
				Cover      string `json:"cover"`
				ViewCount  string `json:"view_count"`
				LikeCount  string `json:"like_count"`
				Duration   string `json:"duration"`
				AuthorName string `json:"author_name"`
				AuthorID   int    `json:"author_id"`
				Bvid       string `json:"bvid"`
			} `json:"archives"`
		} `json:"archive_channels"`
	} `json:"data"`
}

type VideoResp struct {
	Code    int    `json:"code"`
	Message string `json:"message"`
	TTL     int    `json:"ttl"`
	Data    struct {
		Offset  string `json:"offset"`
		HasMore bool   `json:"has_more"`
		List    []struct {
			CardType     string `json:"card_type"`
			PublishRange int    `json:"publish_range,omitempty"`
			UpdateTime   int    `json:"update_time,omitempty"`
			Title        string `json:"title,omitempty"`
			Items        []struct {
				ID         int    `json:"id"`
				Name       string `json:"name"`
				Cover      string `json:"cover"`
				ViewCount  string `json:"view_count"`
				LikeCount  string `json:"like_count"`
				Duration   string `json:"duration"`
				AuthorName string `json:"author_name"`
				AuthorID   int    `json:"author_id"`
				Bvid       string `json:"bvid"`
				Sort       string `json:"sort"`
				Filt       int    `json:"filt"`
			} `json:"items,omitempty"`
			ID         int    `json:"id,omitempty"`
			Name       string `json:"name,omitempty"`
			Cover      string `json:"cover,omitempty"`
			ViewCount  string `json:"view_count,omitempty"`
			LikeCount  string `json:"like_count,omitempty"`
			Duration   string `json:"duration,omitempty"`
			AuthorName string `json:"author_name,omitempty"`
			AuthorID   int    `json:"author_id,omitempty"`
			Bvid       string `json:"bvid,omitempty"`
			Sort       string `json:"sort,omitempty"`
			Filt       int    `json:"filt,omitempty"`
		} `json:"list"`
	} `json:"data"`
}

type Video struct {
	ChannelId  int    `gorm:"channel_id"`
	Name       string `gorm:"name"`
	ViewCount  string `gorm:"view_count"`
	LikeCount  string `gorm:"like_count"`
	AuthorName string `gorm:"author_name"`
	AuthorId   int64  `gorm:"author_id"`
	Bvid       string `gorm:"bvid"`
}

type Done struct {
	DoneType int
	Mission  int
}

func GetBiliChannelId(id int, chanChannelId chan<- int, doneChannel chan<- Done) {
	client := http.Client{}
	request, err := http.NewRequest("GET", "https://api.bilibili.com/x/web-interface/web/channel/category/channel_arc/list?id="+strconv.Itoa(id), nil)
	if err != nil {
		// error channel
	}

	resp, err := client.Do(request)
	if err != nil {
		// error channel
	}
	body, _ := ioutil.ReadAll(resp.Body)
	defer resp.Body.Close()

	var res Catgory
	// 获取结果
	if err := json.Unmarshal(body, &res); err != nil {
		// error chanel
	}

	count := 0
	for _, v := range res.Data.ArchiveChannels {
		chanChannelId <- v.ID
		count++
	}
	doneChannel <- Done{
		DoneType: 0,
		Mission:  count, // 第二层的任务数
	}
}

// 二层并发，网络请求 视频信息
func GetBiliVideo(ChannelId int, chanVideo chan<- Video, doneChannel chan<- Done) {
	client := http.Client{}
	request, err := http.NewRequest("GET", "https://api.bilibili.com/x/web-interface/web/channel/multiple/list?channel_id="+strconv.Itoa(ChannelId)+"&sort_type=hot&page_size=30", nil)
	if err != nil {
		// error channel
	}

	resp, err := client.Do(request)
	if err != nil {
		// error channel
	}

	body, _ := ioutil.ReadAll(resp.Body)
	defer resp.Body.Close()

	var res VideoResp
	if err := json.Unmarshal(body, &res); err != nil {
		// error channel
	}

	count := 0
	for _, v := range res.Data.List {
		count++
		chanVideo <- Video{
			ChannelId:  ChannelId,
			Name:       v.Name,
			ViewCount:  v.ViewCount,
			LikeCount:  v.LikeCount,
			AuthorName: v.AuthorName,
			AuthorId:   int64(v.AuthorID),
			Bvid:       v.Bvid,
		}
	}

	doneChannel <- Done{
		DoneType: 1,
		Mission:  count,
	}
}

// 三层并发
func InsertDatabase(video Video) {
	item := Video{
		ChannelId:  video.ChannelId,
		Name:       video.Name,
		ViewCount:  video.ViewCount,
		LikeCount:  video.LikeCount,
		AuthorName: video.AuthorName,
		AuthorId:   video.AuthorId,
		Bvid:       video.Bvid,
	}

	if err := DB.Create(&item).Error; err != nil {
		// error channel
	}
}

func main() {
	// 替换 yourname 和 yourpassword
	DB, _ = gorm.Open("mysql", "yourname:yourpassword@/video?parseTime=True")

	// 初始化 mission1, 必须初始化
	mission1 = 22

	chanChannelId := make(chan int, 100)
	chanVideo := make(chan Video, 100)
	errChannel := make(chan bool, 10)
	doneChannel := make(chan Done, 10)

	for i := 1; i <= 22; i++ {
		go GetBiliChannelId(i, chanChannelId, doneChannel)
	}

	defer close(chanChannelId)
	defer close(chanVideo)
	defer close(errChannel)
	defer close(doneChannel)

	t := time.Now()
	for !done1 || !done2 || !done3 {
		select {
		case channelId := <-chanChannelId:
			// 通过一个 channelId 获取 n 个 video
			go GetBiliVideo(channelId, chanVideo, doneChannel)
		case video := <-chanVideo:
			// 三层，插入数据库
			InsertDatabase(video)
			mission3--
			if done1 && done2 && mission3 == 0 {
				done3 = true
			}
		case err := <-errChannel:
			// 错误处理
			fmt.Println(err)
		case done := <-doneChannel:
			switch done.DoneType {
			case 0: // 第一层并发
				mission1--
				mission2 += done.Mission
				if mission1 == 0 { // 任务 1 结束
					done1 = true
				}
			case 1: // 第二层并发
				mission2--
				mission3 += done.Mission
				if done1 && mission2 == 0 {
					done2 = true
				}
			}
		}
	}
	fmt.Printf("\n\n%v\n\n", time.Since(t))
}

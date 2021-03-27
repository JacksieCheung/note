USE `video`

DROP TABLE IF EXISTS `videos`;
create table `videos`(
  `id`          int          not null auto_increment ,
  `channel_id`  int          not null ,
  `view_count`  varchar(100) not null ,
  `name`        varchar(100) not null ,
  `like_count`  varchar(100) not null ,
  `author_name` varchar(100) not null ,
  `author_id`   int          not null ,
  `bvid`        varchar(100) not null ,
  primary key (`id`)
)ENGINE=InnoDB;

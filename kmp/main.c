#include<stdio.h>

int next[100];

void GetNext(char T[])
{
	int j=0,k=-1;
	next[j]=k;
	while(T[j]!='\0')
	{
		if(k==-1||T[j]==T[k])
		{
			j++;
			k++;
			next[j]=k;
		}
		else k=next[k];
	}
}

int KMP(int start,char S[],char T[])
{
	int i=start,j=0;
	while(j==-1||(S[i]!='\0'&&T[j]!='\0'))
	{
		printf("before:(%d,%d)\n",i,j);
		if(j==-1||S[i]==T[j])
		{
			i++;         //继续对下一个字符比较 
			j++;         //模式串向右滑动 
		}
		else j=next[j];
		printf("after:(%d,%d)\n",i,j);
	}
	if(T[j]=='\0'){
		return (i-j);    //匹配成功返回下标 
	}
	else {
		return -1;                 //匹配失败返回-1 
	}
}

int main(){
	char s[100] = "hellohellohihello";
	char t[5] = "hih";
	GetNext(t);
	printf("%d\n",KMP(0,s,t));
}

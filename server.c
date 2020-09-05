/*server.c*/

/*该文件创建了一个服务器端，并且会创建一个目录文件*/
/*在客户端链接到服务器的时候将该目录文件传送给客户端作为声明*/

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>
#include<time.h>



/*mode方式,代表：该文件所有者拥有读，写和执行操作的权限，该文件用户组拥有可读、可执行的权限，其他用户拥有可读、可执行的权限。*/
#define MODE S_IRWXU | S_IRGRP | S_IXGRP | S_IROTH | S_IXOTH

#define mode S_IRWXU|S_IRWXG|S_IRWXO

#define MAXSIZE 1024     /*定义数据缓冲区大小*/

void init();
int judge(char *str,char *ptr1,char *ptr2);
void getUsernameAndPsd(char *username,char *ptr1,char *ptr2);
void server_upload(char *buffer,int new_fd);
void server_preview_and_download(char *buffer,int new_fd,char *username);
void displayhistory(char *username,int new_fd);
int findFromFileTable(char *username,char *ptr);
void renewHistory(char *username,char *ptr,char *commit);
void creatHistory(char *username);
void createfoler(char *thisleveldir,char *ptr);
void renewFileTable(char *supleveldir,char *ptr);
void creatRecord(char *supleveldir,char *ptr);
void renewRecord(char *username,char *ptr);
char *getFileName(char *ptr);
char *getFullFileName1(char *username,char *ptr);
char *getFullFileName2(char *username,char *ptr,char *time1);
void getTime();
void split1(char *str,char *username,char *filename,char *commit);
void split2(char *str,char *filename,char *timetemp);

char usertable[]="usertable.txt";
char filetable[]="filetable.txt";
char record[]="record.txt";
char history[]="history.txt";

char time1[2*MAXSIZE];

/*记得修改！！！！！！！*/
//char pathname[1024]="/home/yisong/桌面/";
/*记得修改！！！！！！！*/

char pathname[1024];

char *username;
char *filename;
char *timetemp;
char *commit;

int main(int argc, char *argv[])
{
	int sockfd,new_fd;
	struct sockaddr_in server_addr;   /*定义服务器端套接口数据结构server_addr */
	struct sockaddr_in client_addr;   /*定义客户端套接口数据结构client_addr */
	int sin_size,portnumber; 
	char buffer[MAXSIZE];	   /*发送数据缓冲区*/
	int num;		/*记录发送给对方的文件长度，可以输出该变量判断传输文件的完整性*/		
	int n;			/*记录写入缓存区的文件长度，可以输出该变量判断文件是否完整导入缓冲区*/
	FILE *fp;
	int fd;
	
	init();
	
	if(argc!=3)		/*当输入参数不是3个时返回错误信息*/
	{
		fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
		exit(1);
	}
	if((portnumber=atoi(argv[1]))<0)	/*获得命令行的第二个参数--端口号（不小于0），atoi()把字符串转换成整型数*/
	{  
		fprintf(stderr,"Usage:%s portnumber\a\n",argv[0]);
		exit(1);
	}
	strcpy(pathname,argv[2]);
	
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1)/*服务器端开始建立socket描述符*/
	{
		fprintf(stderr,"Socket error:%s\n\a",strerror(errno));	/*错误返回，不缓存直接输出在屏幕上*/
		exit(1);
	}
	else
	{
		printf("socket created successfully!\n");	/*保留作套接字创建是否成功测试*/
		printf("socket id:%d\n",sockfd);
	}
	/*服务器端填充 sockaddr结构*/
	bzero(&server_addr,sizeof(struct sockaddr_in)); /*先将套接口地址数据结构清零*/
	server_addr.sin_family=AF_INET;/*设为TCP/IP地址族*/
	server_addr.sin_addr.s_addr=htonl(INADDR_ANY);/*设为本机地址*/
	server_addr.sin_port=htons(portnumber);/*设置将要绑定的接口*/
	if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1)/*调用bind函数绑定端口*/
	{
		fprintf(stderr,"Bind error:%s\n\a",strerror(errno));	/*错误返回，不缓存直接输出在屏幕上*/
		exit(1);
	}
	else
	{
		printf("bind port successfully!\n");	/*保留作接口绑定判断*/
		printf("local port:%d\n",portnumber);
	}
	
	if(listen(sockfd,5)==-1)  /*端口绑定成功，监听sockfd描述符，同时处理的最大连接请求数为5 */
	{
		fprintf(stderr,"Listen error:%s\n\a",strerror(errno));
		exit(1);
	}
	while(1)      /*服务器阻塞，等待接收连接请求，直到客户程序发送连接请求*/
	{
		sin_size=sizeof(struct sockaddr_in);
		if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1)		/*调用accept接受一个连接请求*/
		{
			fprintf(stderr,"Accept error:%s\n\a",strerror(errno));
			exit(1);
		}
		fprintf(stderr,"Server get connection from %s\n",(inet_ntoa(client_addr.sin_addr)));
		/*TCP连接已建立，打印申请连接的客户机的IP地址*/
					
		if(fork() == 0)
		{
			char username[20];
			char password[20];
			memset(username, 0x00, sizeof (username)); 
			memset(password, 0x00, sizeof (password)); 
			
			while(1)
			{
				
				memset(buffer, 0x00, sizeof (buffer));
				if(read(new_fd,buffer,1024)!=-1)
				{
					printf("%s\n",buffer);
					int n=judge(buffer,username,password);
					if(n)
					{
						printf("%d\n",n);
						strcpy(buffer,"correct");
						write(new_fd,buffer,strlen(buffer));
						break;
					}
					else
					{
						printf("%d\n",n);
						strcpy(buffer,"error");
						write(new_fd,buffer,strlen(buffer));
						break;
					}
				}
			}
			
			displayhistory(username,new_fd);
			
			while(1)
			{
				if((num=read(new_fd,buffer,1024))>0)
				{
					buffer[num]='\0';
					printf("%s\n",buffer);			
					server_upload(buffer,new_fd);
					server_preview_and_download(buffer,new_fd,username);
				}
				memset(buffer, 0x00, sizeof (buffer));
				if(read(new_fd,buffer,1024)>0)
				{
					printf("%s\n",buffer);
					if(strcmp(buffer,"NOMORE")==0)
					{
						strcpy(buffer,"bye");
						write(new_fd,buffer,strlen(buffer));//向客户端回复消息
						printf("success all\n");
						break;
					}
				}
			}
			
			close(new_fd);  /*本次通信已结束，关闭客户端的套接口，并循环下一次等待*/
			exit(0);
		}
		else
		{
			close(new_fd);/*本次通信已结束，关闭客户端的套接口，并循环下一次等待*/
		}
		
	}
	close(sockfd);  /*服务器进程结束，关闭服务器端套接口*/
	exit(0);
}

void init()
{
	char buf[40];  /*缓冲区*/
	FILE *fp;            /*文件指针*/
	int len;             /*行字符个数*/
	int i=1;		/*记录当前是奇数行还是偶数行*/
	if((fp = fopen(usertable,"r")) == NULL)
	{
		perror("fail to read");
		exit (1) ;
	}
	while(fgets(buf,20,fp) != NULL)
	{
		if(i%2==0)
		{
			i++;
			continue;
		}
		len = strlen(buf);
		buf[len-1] = '\0';  /*去掉换行符*/
		mkdir(buf,MODE);
		i++;
	}
	fclose(fp);
}

int judge(char *str,char *ptr1,char *ptr2)
{
	//char str[]="username:123456";
	char getUsername[20];
	char getPassword[20];
	memset(getUsername, 0x00, sizeof (getUsername)); 
	memset(getPassword, 0x00, sizeof (getPassword));
	char delims[]=":";
	char *result=NULL;
	int i=1;
	result=strtok(str,delims);
	while(result!=NULL)
	{
		if(i==1)
		{
			strcpy(getUsername,result);		
		}
		else
		{
			strcpy(getPassword,result);
		}
		result=strtok(NULL,delims);
		i++;
	}
	getUsernameAndPsd(getUsername,ptr1,ptr2);
	
	if((strcmp(getUsername,ptr1)==0)&&(strcmp(getPassword,ptr2)==0))
	{
		return 1;
	}
	else
	return 0;
}

void getUsernameAndPsd(char *username,char *ptr1,char *ptr2)
{
	char buf[40];  /*缓冲区*/
	FILE *fp;            /*文件指针*/
	int len;             /*行字符个数*/
	memset(buf, 0x00, sizeof (buf)); 
	if((fp = fopen(usertable,"r")) == NULL)
	{
		perror("fail to read");
		exit (1) ;
	}
	while(fgets(buf,40,fp) != NULL)
	{
		//printf("%s\n",buf);
		len = strlen(buf);
		buf[len-1] = '\0';  /*去掉换行符*/
		if(strcmp(buf,username)==0)
		{
			strcpy(ptr1,buf);
			if(fgets(buf,40,fp) != NULL)
			{
				//printf("%s\n",buf);
				len = strlen(buf);
				buf[len-1] = '\0';  /*去掉换行符*/
				strcpy(ptr2,buf);
				break;
			}
		}
	}
	fclose(fp);
	printf("%s\n%s\n",ptr1,ptr2);
}

void server_upload(char *buffer,int new_fd)
{
	FILE *fp;
	int num;
	char delims[]="*";
	
	if(strcmp(buffer,"1.upload file")==0)
	{
		memset(buffer, 0x00, sizeof (buffer));
		while(1)
		{
			if(read(new_fd,buffer,1024)>0)
			{
				//split1(buffer,username,filename,commit);
				
				char delims[]="*";
				char *result=NULL;
				int i=1;
				int num;
				result=strtok(buffer,delims);
				while(result!=NULL)
				{
					if(i==1)
					{
						num=strlen(result);
						result[num]='\0';
						username=result;			
					}
				else if(i==2)
				{
					num=strlen(result);
					result[num]='\0';
					filename=result;
				}
				else if(i==3)
				{
					num=strlen(result);
					result[num]='\0';
					commit=result;
				}
				result=strtok(NULL,delims);
				i++;
			}
			printf("%s\n",username);
			printf("%s\n",filename);
			printf("%s\n",commit);	
				
			break;
			}
		}
		
		printf("%s\n",username);
		/*
		printf("%s\n",filename);
		printf("%s\n",commit);*/
		
		if(findFromFileTable(username,filename)==0)
		{
			printf("99999988888888888\n");
			renewRecord(username,filename);
		}
		printf("999999\n");
		renewHistory(username,filename,commit);
		printf("455687676\n");
		
		char *fullFileName;
		char filename2[MAXSIZE];
		fullFileName=getFullFileName1(username,filename);
		strcpy(filename2,fullFileName);
		
		printf("------------------------\n");
		printf("%s\n",fullFileName);
		
		memset(buffer, 0x00, sizeof (buffer));
		strcpy(buffer,"Ready");
		printf("%s\n",buffer);
		num = write(new_fd,buffer,strlen(buffer));
		printf("111111111111111\n");
		printf("%d\n",num);
		
		
		while(1)
		{
			if(read(new_fd,buffer,1024)>0)
			{
				printf("%s\n",buffer);
				if(strcmp(buffer,"End")==0)
				{
					strcpy(buffer,"success");
					num = write(new_fd,buffer,strlen(buffer));//向客户端回复消息
					printf("%d\n",num);
					printf("upload success\n");
					break;
				}
				printf("收到客户端消息：%s\n",buffer);
				if( (fp = fopen(filename2, "a+")) == NULL)
				{
					printf ("fail to open txt!\n");
					exit(1);          /*出错退出*/
				}
				fprintf(fp,"%s\n",buffer);
				memset(buffer, 0x00, sizeof (buffer));
				strcpy(buffer,"OK");
				num = write(new_fd,buffer,strlen(buffer));//向客户端回复消息
				printf("%d\n",num);
				fclose(fp);
			}
		}		
	}	
}

void server_preview_and_download(char *buffer,int new_fd,char *username)
{
	int fd;
	int num;
	char fileNameTemp[MAXSIZE];
	FILE *fp;
	char delims[]="*";
	
	char information[1024];
	strcpy(information,buffer);
	if(strcmp(information,"2.preview file")==0)
	{
		memset(buffer, 0x00, sizeof (buffer));
		while(1)
		{
			if(read(new_fd,buffer,1024)>0)
			{
				char delims[]="*";
				char *result=NULL;
				int i=1;
				int num;
				result=strtok(buffer,delims);
				while(result!=NULL)
				{
					if(i==1)
					{
					num=strlen(result);
					result[num]='\0';
					filename=result;			
				}
				else if(i==2)
				{
					num=strlen(result);
					result[num]='\0';
					timetemp=result;
				}
				result=strtok(NULL,delims);
				i++;
			}
			break;
			}
		}
		printf("%s\n",filename);
		printf("%s\n",timetemp);
		printf("1111\n");
		char *fullFileName;
		char buffer1[2*MAXSIZE];
		memset(buffer1, 0x00, sizeof (buffer1));
		fullFileName=getFullFileName2(username,filename,timetemp);
		strcpy(buffer1,pathname);
		printf("%s\n",buffer1);
		strcat(buffer1,username);
		printf("%s\n",buffer1);
		strcat(buffer1,"/");
		printf("%s\n",buffer1);
		strcat(buffer1,fullFileName);
		printf("%s\n",buffer1);
		
		if((fp=fopen(buffer1,"r"))==NULL)	//打开指定文件
		{
			perror("fail to read");
			strcpy(information,"Error");
			write(new_fd,information,1024);
			exit(1);
		}
		memset(information, 0x00, sizeof (information));
		strcpy(information,"Ready");
		printf("%s\n",information);
		num = write(new_fd,information,strlen(information));
		printf("%d\n",num);
		while(fgets(buffer,1024,fp) != NULL)
		{
			int len = strlen(buffer);
			buffer[len-1] = '\0';  /*去掉换行符*/
			//printf("%s\n",buffer);
			write(new_fd,buffer,1024);
			memset(buffer, 0x00, sizeof (buffer));
			if(read(new_fd,buffer,1024)>0)	
			{
				printf("%s\n",buffer);
			}
		}
		memset(information, 0x00, sizeof (information));
		strcpy(information,"End");	//向客户端发送“END”表示此次文件传输完
		write(new_fd,information,strlen(information));
		fclose(fp);
	}
}

void displayhistory(char *username,int new_fd)
{
	char buffer[MAXSIZE];
	FILE *fp;
	int num;
	int fd;
	
	strcpy(buffer,pathname);
	strcat(buffer,username);
	strcat(buffer,"/");
	strcat(buffer,history);
	
	if(fd=open(buffer,O_RDONLY,mode)<0)
	{
		creatHistory(username);
	}
	
	printf("%s\n",buffer);
	if((fp = fopen(buffer,"r")) == NULL)
  	{
    	printf("error");
   		exit (1) ;
  	}
	
	memset(buffer, 0x00, sizeof (buffer));
	if((num=read(new_fd,buffer,1024))>0)
	{
		if(strcmp(buffer,"Ready")==0)		//如果得到Ready的答复
		{
			memset(buffer, 0x00, sizeof (buffer));
			while(fgets(buffer,1024,fp) != NULL)
			{
				write(new_fd,buffer,strlen(buffer));
				printf("%s\n",buffer);
				memset(buffer, 0x00, sizeof (buffer));
				if(read(new_fd,buffer,1024)>0)
				{
					printf("%s\n",buffer);		//接收到客户端发送过来的如"OK"的消息
					memset(buffer, 0x00, sizeof (buffer));
				}
			}
			memset(buffer, 0x00, sizeof (buffer));
			strcpy(buffer,"End");
			write(new_fd,buffer,strlen(buffer));//向客户端发送消息
		}
	}
	
	fclose(fp);
}

int findFromFileTable(char *username2,char *ptr)	/*ptr为客户端传送给服务器的文件名字*/
{
  	char buf[MAXSIZE];  /*缓冲区*/
	char thisleveldir[2*MAXSIZE];/*本级目录的名字*/
	char filename2[2*MAXSIZE];/*拼接好的文件名字*/
  	FILE *fp1;            /*文件指针*/
  	int len;             /*行字符个数*/
	int fd;
	
	printf("3333332233333\n");
	printf("%s\n",username2);
	
	strcpy(buf,pathname);
	strcat(buf,username2);
	printf("%s\n",buf);
	strcpy(thisleveldir,buf);/*保存下本级目录的名字*/
	printf("%s\n",thisleveldir);
	strcat(buf,"/");
	strcat(buf,filetable);
	printf("%s\n",buf);
	printf("3333222333\n");
	strcpy(filename2,buf);
	printf("%s\n",filename2);
	printf("3333222333\n");
	/*
	if(fd = open(filename2,O_CREAT,mode)<0)
	{
		printf("err\n");
	}
	printf("13255\n");
	close(fd);
	strcpy(buf,"touch ");
	strcat(buf,filename2);
	system(buf);
	*/
	
	printf("333322233344444444\n");
  	if((fp1 = fopen("index111.txt","a+")) == NULL)
  	{
    	perror("fail to read");
   		exit (1) ;
  	}
	else
	{
		printf("231654645\n");
	}
	printf("3333333333333\n");
	memset(buf, 0x00, sizeof (buf));
  	while(fgets(buf,MAXSIZE,fp1) != NULL)
  	{
   		len = strlen(buf);
    	buf[len-1] = '\0';  /*去掉换行符*/
   		if(strcmp(buf,ptr)==0)
   		{
			renewRecord(username,ptr);
			fclose(fp1);
   			return 1; 
   		}
  	}
  	createfoler(thisleveldir,ptr);
	fclose(fp1);
	printf("findFromFileTable end\n");
	return 0;		
}

void renewHistory(char *username,char *ptr,char *commit)/*ptr为客户端传送给服务器的文件名字*/
{
	char buffer[MAXSIZE];
	char content[MAXSIZE];
	FILE *fp;
	
	strcpy(buffer,pathname);
	strcat(buffer,username);
	strcat(buffer,"/");
	strcat(buffer,history);
	
	printf("4865446\n");
	
	if((fp = fopen(buffer,"a+")) == NULL)
  	{
    	perror("fail to read");
   		exit (1) ;
  	}
	
	/*向history.txt中输入类似于 admin*Fri Oct 18 18:59:12 2019*file111的记录*/
	strcpy(content,username);
	strcat(content,"*");
	strcat(content,time1);
	strcat(content,"*");
	strcat(content,ptr);
	strcat(content,"*");
	strcat(content,commit);
	
	fprintf(fp,"%s\n",content);
	fclose(fp);
	printf("renew history.txt success!\n");
}

void creatHistory(char *username)
{
	char buffer[MAXSIZE];
	FILE *fp;
	
	strcpy(buffer,pathname);
	strcat(buffer,username);
	strcat(buffer,"/");
	strcat(buffer,history);
	
	printf("%s\n",buffer);
	
	if((fp = fopen(buffer,"w+")) == NULL)
  	{
    	perror("fail to read");
   		exit (1) ;
  	}
	
	fclose(fp);
	printf("create history.txt success!\n");
	
}

void createfoler(char *thisleveldir,char *ptr)/*thisleveldir为指向本级目录字符串的指针*/
{
	char buf[MAXSIZE];
	memset(buf, 0x00, sizeof (buf));
	strcpy(buf,thisleveldir);
	strcat(buf,"/");
	strcat(buf,ptr);
	printf("%s\n",buf);
	if(mkdir(buf,MODE)!=-1)
	{
		printf("创建文件夹成功\n");
		renewFileTable(thisleveldir,ptr);		/*ptr为客户端传送给服务器的文件名字*/
		
		/*在每个文件夹中创建record.txt*/
		creatRecord(thisleveldir,ptr);
	}
}

void renewFileTable(char *supleveldir,char *ptr)
{
	FILE *fp;
	char buf[MAXSIZE];
	strcpy(buf,supleveldir);
	strcat(buf,"/");
	strcat(buf,filetable);
	
	if( (fp = fopen(buf, "a+")) == NULL)
	{
		printf ("fail to open txt!\n");
		exit(1);          /*出错退出*/
	}
	printf("1111112222222222\n");
	printf("%s\n",ptr);
	fprintf(fp,"%s\n",ptr);
	fclose(fp);
	printf("renew filetable success\n");
}

void creatRecord(char *supleveldir,char *ptr)
{
	FILE *fp;
	char buf[MAXSIZE];
	strcpy(buf,supleveldir);
	strcat(buf,"/");
	strcat(buf,ptr);
	strcat(buf,"/");
	strcat(buf,record);
	printf("%s\n",buf);
	
	if( (fp = fopen(buf, "w+")) == NULL)
	{
		printf ("fail to open txt!\n");
		exit(1);          /*出错退出*/
	}
	fclose(fp);
	printf("creat record.txt success\n");
}

void renewRecord(char *username,char *ptr)		/*ptr为客户端传送给服务器的文件名字*/
{
	FILE *fp;
	char buf[MAXSIZE];
	char *fileName;
	strcpy(buf,pathname);
	strcat(buf,username);
	strcat(buf,"/");
	strcat(buf,ptr);
	strcat(buf,"/");
	strcat(buf,record);
	
	printf("\n\n%s\n\n",buf);
  	if((fp = fopen(buf,"w+")) == NULL)
  	{
    	perror("fail to read");
   		exit (1) ;
  	}
	
	printf("rereererer\n");
	fileName=getFileName(ptr);
	printf("rewwwwwwwwwwwwwreererer\n");
	fprintf(fp,"%s\n",fileName);
	printf("%s\n",filename);
	printf("renewRecord success!\n");
	fclose(fp);
}

/*拼接成类似于 file1 Fri Oct 18 18:59:12 2019.txt 的文件名字*/
char *getFileName(char *ptr)
{
	char FileName[2*MAXSIZE];
	getTime();
	printf("getgetget\n");
	
	strcpy(FileName,ptr);
	strcat(FileName," ");
	strcat(FileName,time1);
	strcat(FileName,".txt");
	printf("%s\n",FileName);
	
	printf("getgetget success\n");
	char *p=FileName;
	return p;
}

char *getFullFileName1(char *username,char *ptr)
{
	printf("4564646\n");
	char FileName[2*MAXSIZE];
	strcpy(FileName,pathname);
	strcpy(FileName,username);
	printf("95555555\n");
	strcat(FileName,"/");
	strcat(FileName,ptr);
	printf("96666666\n");
	strcat(FileName," ");
	printf("9777777\n");
	strcat(FileName,time1);
	printf("988888\n");
	strcat(FileName,".txt");
	printf("9999999999\n");
	printf("%s\n",FileName);
	char *p=FileName;
	printf("getFullFileName1 success\n");
	return p;
}

char *getFullFileName2(char *username,char *ptr,char *time1)
{
	printf("11111111111111\n");
	char FileName[MAXSIZE];
	strcpy(FileName,pathname);
	strcat(FileName,username);
	strcat(FileName,"/");
	strcpy(FileName,ptr);
	strcat(FileName," ");
	strcat(FileName,time1);
	strcat(FileName,".txt");
	printf("%s\n",FileName);
	char *p=FileName;
	return p;
}

void getTime()
{
	time_t rawtime;
	struct tm* timeinfo;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	printf("%s",asctime(timeinfo));
	strcpy(time1,asctime(timeinfo));
	int num=strlen(time1);
	time1[num-1]='\0';
	printf("getTime success\n");
}

void split1(char *str,char *username,char *filename,char *commit)
{
	char delims[]="*";
	char *result=NULL;
	int i=1;
	int num;
	result=strtok(str,delims);
	while(result!=NULL)
	{
		if(i==1)
		{
			num=strlen(result);
			result[num]='\0';
			username=result;			
		}
		else if(i==2)
		{
			num=strlen(result);
			result[num]='\0';
			filename=result;
		}
		else if(i==3)
		{
			num=strlen(result);
			result[num]='\0';
			commit=result;
		}
		result=strtok(NULL,delims);
		i++;
	}
	printf("%s\n",username);
	printf("%s\n",filename);
	printf("%s\n",commit);
}

void split2(char *str,char *filename,char *timetemp)
{
	char delims[]="*";
	char *result=NULL;
	int i=1;
	int num;
	result=strtok(str,delims);
	while(result!=NULL)
	{
		if(i==1)
		{
			num=strlen(result);
			result[num]='\0';
			filename=result;			
		}
		else if(i==2)
		{
			num=strlen(result);
			result[num]='\0';
			timetemp=result;
		}
		result=strtok(NULL,delims);
		i++;
	}
	printf("%s\n",filename);
	printf("%s\n",timetemp);
}

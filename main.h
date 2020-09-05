#include<gtk/gtk.h>
#include<stdlib.h>
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
#include<signal.h>

/*******窗口类全局变量************/
GtkWidget *entry1;
GtkWidget *entry2;
GtkWidget *main_window;
GtkWidget *progress_window;
GtkWidget *user_window;
GtkWidget *wrong_window;
GtkWidget *timeout_window;
GtkWidget *unreachable_window;
GtkWidget *detail_window;
GtkWidget *entry3;
GtkWidget *entry4;
GtkWidget *entry5;
GtkWidget *label_max;
GtkWidget *entry_port;
GtkWidget *entry_ip;
static GtkWidget *clist_1;
static GtkWidget *clist_2;
static GtkWidget *add_win;
static GtkWidget *entry_time;
static GtkWidget *entry_filepath;
static GtkWidget *entry_branch;
static GtkWidget *entry_commit;
gint current_row = 0;
gint row_count = 0;
/***************************/

/*********函数声明*************/
void create_progress_window();
void create_main_window();
void create_user_window();
void create_wrong_window();
void create_timeout_window();
void destroy_timeout_window();
void destroy_wrong_window();
void create_unreachable_window();
void TCPconnect(char *addr,char *portNumber);
void waiting_for_server();
int client_upload(char *filename);
void create_detail_window();
GtkWidget *create_addwin();
GtkWidget *create_button(gchar *stockid);
void append_row(GtkWidget *button, gpointer data);

/******************************/

/***********全局变量************/
int sockfd;
char text[50];
char filepath[50];
char server_addr[20];
char server_port[20];
char buffer[1024];
/***********全局变量************/




/**客户端上传代码函数**/
int client_upload(char *filename)	//参数为文件路径，绝对路径或相对路径
{
	FILE *fp;
	int time = 0;
	int len;
	int num;
	const gchar *new_row_detail[2];
	char information[1024];
	memset(buffer, 0x00, sizeof (buffer));	//清空缓存区
	strcpy(buffer,"1.upload file");	//填入标识符，表示客户端的操作为上传文件
	//num = strlen(buffer);	/*设置读取长度*/
	write(sockfd,buffer,sizeof(buffer));	//向套接口中写入标识，向服务器发起请求
	printf("%s\n",buffer);
	
	if((fp = fopen(filename,"r")) == NULL)	//打开指定文件
	{
		perror("fail to read");
	}
	memset(buffer, 0x00, sizeof (buffer));
	printf("%s\n",buffer);
	while(1)	//保持监听套接口，直到服务器返回ready信号表示达到同步
	{
		time++;
		/*if(time==10)
			break;*/
		printf("%d\n",time);
		printf("111111111111111111\n");
		num = read(sockfd,buffer,1024);
		if(read(sockfd,buffer,1024)>0)
		{
			printf("%d\n",num);
			printf("11111111111111\n");
			printf("%s\n",buffer);
			if(strcmp(buffer,"Ready")==0)
			{
				//printf("11111111111111\n");
				while(fgets(buffer,1024,fp) != NULL)
				{
					printf("11111111111111\n");
					int len = strlen(buffer);
					buffer[len-1] = '\0';  /*去掉换行符*/
					stpcpy(information,buffer);
					new_row_detail[1] = information;
					//printf("%s\n",information);
					write(sockfd,buffer,1024);
					if(read(sockfd,buffer,1024)>0)
					{
						new_row_detail[0] = "OK";
						row_count++;
						gtk_clist_append(GTK_CLIST(clist_2), (gchar**)(new_row_detail));
					}
					else
					{
						new_row_detail[0] = "ERROR";
						row_count++;
						gtk_clist_append(GTK_CLIST(clist_2), (gchar**)(new_row_detail));
					}
				}
				strcpy(buffer,"End");
				write(sockfd,buffer,sizeof(buffer));
				
				break;
			}
		}
		usleep(500);
	}
	fclose(fp);
	time = 0;
	while(1)
	{
		time++;
		if(read(sockfd,buffer,1024)>0)
		{
			if(strcmp(buffer,"success")==0)
				return 0;
		}
		if(time==100)
			return 1;
	}
}

void goto_first(GtkWidget *button, gpointer data)
{
    current_row = 0;
    gtk_clist_select_row(GTK_CLIST(clist_2), current_row, 0);
}

 

void goto_last(GtkWidget *button, gpointer data)
{
    current_row = row_count - 1;
    gtk_clist_select_row(GTK_CLIST(clist_2), current_row, 0);

}

void goto_back(GtkWidget *button, gpointer data)

{
    current_row--;
    if (current_row == -1) return;
    gtk_clist_select_row(GTK_CLIST(clist_2), current_row, 0);
}

 

void goto_forward(GtkWidget *button, gpointer data)
{
    current_row++;
    if (current_row > row_count) return;
    gtk_clist_select_row(GTK_CLIST(clist_2), current_row, 0);
}

void on_cancel_clicked_detail(GtkWidget *button, gpointer data)
{
    gtk_widget_destroy(detail_window);
}

void create_detail_window()	//详细信息界面
{
	//GtkWidget *table;
	GtkWidget *vbox;
	GtkWidget *scrlled_window;
	static gchar *titles[2] = {"上传状态"," 代码 "};
	detail_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(detail_window),g_locale_to_utf8("详细信息",-1,NULL,NULL,NULL));
	//table = gtk_table_new(2,2,FALSE);
	scrlled_window = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrlled_window),GTK_POLICY_AUTOMATIC,GTK_POLICY_ALWAYS);
	vbox = gtk_vbox_new(FALSE, 0);
    g_signal_connect(G_OBJECT(detail_window),"delete_event",G_CALLBACK(on_cancel_clicked_detail),NULL);
    clist_2 = gtk_clist_new_with_titles(2, titles);
    gtk_box_pack_start(GTK_BOX(vbox), clist_2, TRUE, TRUE, 5);	
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(scrlled_window),vbox);
	gtk_container_add(GTK_CONTAINER(detail_window), scrlled_window);
	gtk_window_set_default_size(GTK_WINDOW(detail_window), 1000, 500);

	gtk_window_set_position( GTK_WINDOW(detail_window),GTK_WIN_POS_CENTER_ON_PARENT);
	gtk_widget_show_all (detail_window);
}


GtkWidget *create_button(gchar *stockid)
{
    GtkWidget *button;
    GtkWidget *image;
    image = gtk_image_new_from_stock(stockid, GTK_ICON_SIZE_MENU);
    button = gtk_button_new();
    gtk_container_add(GTK_CONTAINER(button), image);
    return button;
}

 

void append_row(GtkWidget *button, gpointer data)
{
    add_win = create_addwin();
}

void on_ok_clicked(GtkWidget *button, gpointer data)
{
	const gchar *new_row[5];
	create_progress_window();
	while (gtk_events_pending ()) //这个循环可以实现即时刷新，如果不使用的话在等待超时的时候加载界面因为没有被gtk_main()刷新而不显示，原因是回调函数执行时间过长，没有及时热刷新
	gtk_main_iteration (); 	//gtk_main的单次迭代操作，在本例中对图形界面进行刷新
	int flag =0;
	create_detail_window();
	strcpy(filepath,gtk_entry_get_text(GTK_ENTRY(entry_filepath)));
	//gtk_widget_show_all(progress_window);
	printf("%s\n",filepath);
	gtk_widget_show_all(progress_window);
	flag=client_upload(filepath);
	if(flag == 1)
	{
		gtk_widget_destroy(progress_window);
		create_timeout_window();
		return ;
	}
	//detail_add();
	//gtk_widget_show_all (detail_window);
	new_row[0] = gtk_entry_get_text(GTK_ENTRY(entry1));
	new_row[1] = gtk_entry_get_text(GTK_ENTRY(entry_filepath));
    new_row[2] = gtk_entry_get_text(GTK_ENTRY(entry_time));
    new_row[3] = gtk_entry_get_text(GTK_ENTRY(entry_branch));
    new_row[4] = gtk_entry_get_text(GTK_ENTRY(entry_commit));
    row_count++;
    gtk_clist_append(GTK_CLIST(clist_1), (gchar**)(new_row));
	gtk_widget_destroy(progress_window);//hide_all
    gtk_widget_destroy(add_win);
}

void on_cancel_clicked(GtkWidget *button, gpointer data)
{
    gtk_widget_destroy(add_win);
}



GtkWidget *create_addwin()
{
    GtkWidget *win;
    GtkWidget *vbox;
    GtkWidget *table;
    GtkWidget *bbox;
    GtkWidget *label;
    GtkWidget *button;
    win = gtk_dialog_new_with_buttons("文件上传",GTK_WINDOW(user_window),GTK_DIALOG_MODAL,NULL);
    //win = gtk_dialog_new();
    //gtk_window_set_title(GTK_WINDOW(win),"添加记录");
    gtk_container_set_border_width(GTK_CONTAINER(win),10);
    gtk_window_set_position(GTK_WINDOW(win), GTK_WIN_POS_CENTER);
    g_signal_connect(G_OBJECT(win),"delete_event",G_CALLBACK(gtk_widget_destroy), win);
    vbox = gtk_vbox_new(FALSE, 0);
    //gtk_container_add(GTK_CONTAINER(win), vbox);
    gtk_box_pack_start_defaults(GTK_BOX(GTK_DIALOG(win)->vbox),vbox);
    table = gtk_table_new(4, 2, FALSE);
    gtk_box_pack_start(GTK_BOX(vbox), table, FALSE, FALSE, 5);
    label = gtk_label_new("文件路径");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 0, 1);
    entry_filepath = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), entry_filepath, 1, 2, 0, 1);
    label = gtk_label_new("上传时间");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 1, 2);
    entry_time = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), entry_time, 1, 2, 1, 2);
    /*label = gtk_label_new("");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
    entry_sex = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), entry_sex, 1, 2, 2, 3);*/
    label = gtk_label_new("上传分支");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 2, 3);
    entry_branch = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), entry_branch, 1, 2, 2, 3);
    label = gtk_label_new("说明");
    gtk_table_attach_defaults(GTK_TABLE(table), label, 0, 1, 3, 4);
    entry_commit = gtk_entry_new();
    gtk_table_attach_defaults(GTK_TABLE(table), entry_commit, 1, 2, 3, 4);
    bbox = gtk_hbutton_box_new();
    gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 5);
    gtk_box_set_spacing(GTK_BOX(bbox), 5);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
    button = gtk_button_new_from_stock(GTK_STOCK_OK);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_ok_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);
    button = gtk_button_new_from_stock(GTK_STOCK_CANCEL);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(on_cancel_clicked), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);
    gtk_widget_show_all(win);
    //gtk_dialog_run(GTK_DIALOG(win));
    return win;
}





/**点击触发事件函数**/
void on_clicked(GtkWidget *widget,gpointer data)
{
	strcpy(text,gtk_entry_get_text(GTK_ENTRY(entry1))); //获取用户账号
	strcat(text,":");	//合理构建字符串，以帮助服务器端的字符串分割
	strcat(text,gtk_entry_get_text(GTK_ENTRY(entry2))); //获取用户密码并连接到账号末端
	printf("%s\n",text);
	strcpy(server_addr,gtk_entry_get_text(GTK_ENTRY(entry_ip)));
	strcpy(server_port,gtk_entry_get_text(GTK_ENTRY(entry_port)));
	gtk_widget_hide_all(main_window);	//隐藏main_window窗口
	TCPconnect(server_addr,server_port);	//连接服务器
	//create_user_window();
}


/**服务器连接函数**/
void TCPconnect(char *addr,char *portNumber)	
{
	struct sockaddr_in server_addr;
	char buf[20];
	int num;
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))<0)	/*客户端开始建立socket描述符*/
	{
		perror("socket created error");		/*错误返回，不缓存直接输出在屏幕上*/
		exit(1);
	}
	else
	{
		printf("socket created successfully!\n");
		printf("socket id:%d\n",sockfd);
	}
	/*客户端填充 sockaddr结构*/
	bzero(&server_addr,sizeof(struct sockaddr_in));	/*先将套接口地址数据结构清零*/
	server_addr.sin_family = AF_INET;	/*设为TCP/IP地址族*/
	server_addr.sin_addr.s_addr = inet_addr(addr);/*设为服务器地址*/
	server_addr.sin_port = htons(atoi(portNumber));/*设置将要绑定的接口*/

	if(connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))<0) /*将本地的socket连接到serv_addr所指定的服务器IP与端口号*/
	{
		create_unreachable_window();
		perror("connect error!\n");	/*错误返回*/
		//exit(1);
	}
	else
	{
		printf("connect successfully!\n");
		num = write(sockfd,text,strlen(text));	/*利用write函数将缓冲区文件传送到对方*/		
		waiting_for_server();			//等待服务器处理信息的返回
	}
	
}

/**等待服务器响应函数**/
void waiting_for_server()
{
	char check[10];	//局部变量定义
	int num;
	int time = 0;
	create_progress_window();		//打开进度条窗口
	while(1)
	{	
		while (gtk_events_pending ()) //这个循环可以实现即时刷新，如果不使用的话在等待超时的时候加载界面因为没有被gtk_main()刷新而不显示，原因是回调函数执行时间过长，没有及时热刷新
		gtk_main_iteration (); 	//gtk_main的单次迭代操作，在本例中对图形界面进行刷新*/
		num = read(sockfd,check,10);
		if(strcmp(check,"correct")==0)	/*如果对比成功则表示通过验证，是可用账户*/
		{
			gtk_widget_hide_all(progress_window);
			create_user_window();
			//gtk_label_set_text (GTK_LABEL(label_max),"已和服务器建立连接");
			break;
		}
		if(strcmp(check,"error")==0)	/*如果匹配为error，就说明该账户的账户或密码有误*/
		{
			gtk_widget_destroy(progress_window);
			create_wrong_window();
			gtk_widget_show_all (main_window);
			break;
		}
		time++;
		usleep(5000);
		if(time==1000)	/*超时返回，如果经过这么长时间仍然没有收到服务器的回复，说明网络可能中断*/
		{
			gtk_widget_hide_all(progress_window);
			create_timeout_window();
			break;
		}
	}
}

/**超时窗口的创建和删除*/

void destroy_timeout_window()	//删除函数，按钮建立连接，点击按钮即可退出该窗口，同时显示登录窗口*/
{
	gtk_widget_destroy(timeout_window);
	gtk_widget_show_all (main_window);
}


void create_timeout_window()	//创建函数//
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *button;
	timeout_window = gtk_window_new(GTK_WINDOW_POPUP);	//无状态栏的窗口
	table = gtk_table_new(2,1,TRUE);	//设定表格的行和列
	gtk_container_add(GTK_CONTAINER (timeout_window),table);	//将表格添加进窗口
	label = gtk_label_new(g_locale_to_utf8("连接超时",-1,NULL,NULL,NULL));	//为标签赋值“连接超时”
	button = gtk_button_new_with_label(g_locale_to_utf8("确认",-1,NULL,NULL,NULL));	//为按钮赋值
	gtk_table_attach (GTK_TABLE(table),label,0,1,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);	//设置单元在表格中的位置，7个参数分别为，父表格，子控件，左边界，右边界，上边界，下边界，x轴，y轴，x轴填充，y轴填充
	gtk_table_attach (GTK_TABLE(table),button,0,1,1,2,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	g_signal_connect (G_OBJECT(button),"clicked",G_CALLBACK(destroy_timeout_window),timeout_window);	//设置回调函数，用来实现退出该窗口
	gtk_window_set_position( GTK_WINDOW(timeout_window), GTK_WIN_POS_CENTER_ALWAYS );	//在屏幕中心显示
	gtk_widget_show_all (timeout_window);	//显示窗口和窗口内所有控件
}


/**主机或端口不可达窗口的创建和删除**/

void destroy_unreachable_window()	//删除函数，按钮建立连接，点击按钮即可退出该窗口，同时显示登录窗口*/
{
	gtk_widget_destroy(unreachable_window);
	gtk_widget_show_all (main_window);
}


void create_unreachable_window()	//创建函数//
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *button;
	unreachable_window = gtk_window_new(GTK_WINDOW_POPUP);	//无状态栏的窗口
	table = gtk_table_new(2,1,TRUE);	//设定表格的行和列
	gtk_container_add(GTK_CONTAINER (unreachable_window),table);	//将表格添加进窗口
	label = gtk_label_new(g_locale_to_utf8("主机或端口不可达",-1,NULL,NULL,NULL));	//为标签赋值“连接超时”
	button = gtk_button_new_with_label(g_locale_to_utf8("确认",-1,NULL,NULL,NULL));	//为按钮赋值
	gtk_table_attach (GTK_TABLE(table),label,0,1,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);	//设置单元在表格中的位置，7个参数分别为，父表格，子控件，左边界，右边界，上边界，下边界，x轴，y轴，x轴填充，y轴填充
	gtk_table_attach (GTK_TABLE(table),button,0,1,1,2,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	g_signal_connect (G_OBJECT(button),"clicked",G_CALLBACK(destroy_unreachable_window),unreachable_window);	//设置回调函数，用来实现退出该窗口
	gtk_window_set_position( GTK_WINDOW(unreachable_window), GTK_WIN_POS_CENTER_ALWAYS );	//在屏幕中心显示
	gtk_widget_show_all (unreachable_window);	//显示窗口和窗口内所有控件
}



/**错误账户返回窗口的创建和删除*/

void destroy_wrong_window()
{
	gtk_widget_destroy(wrong_window);
	gtk_widget_show_all (main_window);
}

void create_wrong_window()
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *button;
	wrong_window = gtk_window_new(GTK_WINDOW_POPUP);
	table = gtk_table_new(2,1,TRUE);
	gtk_container_add(GTK_CONTAINER (wrong_window),table);
	label = gtk_label_new(g_locale_to_utf8("账号或密码错误",-1,NULL,NULL,NULL));
	button = gtk_button_new_with_label(g_locale_to_utf8("确认",-1,NULL,NULL,NULL));
	gtk_table_attach (GTK_TABLE(table),label,0,1,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),button,0,1,1,2,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	g_signal_connect (G_OBJECT(button),"clicked",G_CALLBACK(destroy_wrong_window),wrong_window);
	gtk_window_set_position( GTK_WINDOW(wrong_window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_widget_show_all (wrong_window);
}


/**进度条窗口创建**/

void create_progress_window() //创建进度条窗口
{
	GtkWidget *label;
	GtkWidget *table;
	GtkWidget *bar;
	progress_window = gtk_window_new(GTK_WINDOW_POPUP);
	table = gtk_table_new(2,1,TRUE);
	gtk_container_add(GTK_CONTAINER (progress_window),table);
	label = gtk_label_new(g_locale_to_utf8("加载中......",-1,NULL,NULL,NULL));
	gtk_table_attach (GTK_TABLE(table),label,0,1,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	GtkObject *adjustment = gtk_adjustment_new(70.0,0.0,100.0,1.0,0.0,0.0); //范围控件，初始值，最小值，最大值，最小可调值，最大可调值，可视区域
	bar = gtk_progress_bar_new_with_adjustment(GTK_ADJUSTMENT(adjustment));//创建进度条并和范围控件关联，且进度条名称为bar
	gtk_progress_bar_set_bar_style(GTK_PROGRESS_BAR(bar),GTK_PROGRESS_CONTINUOUS);//设置进度条的模式为连续进度条
	gtk_progress_bar_set_orientation(GTK_PROGRESS_BAR(bar), GTK_PROGRESS_LEFT_TO_RIGHT);//设置进度条的方向为从左往右
	
	gtk_table_attach (GTK_TABLE(table),bar,0,1,1,2,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_widget_set_usize(progress_window,300,100);//设置窗口大小
 	gtk_widget_set_size_request(bar,280,10);//设置空间大小
	gtk_window_set_position( GTK_WINDOW(progress_window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_widget_show_all (progress_window);
	//return progress_window;
}

void create_user_window()	//用户界面
{
	GtkWidget *table;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *button;
	GtkWidget *vbox;
	GtkWidget *bbox;
	GtkTooltips *button_tips;
	static gchar *titles[5] = {"  用户名  ","  文件名  ","  上传时间  ","  所在分支  ","  说明  "};
	user_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(user_window),g_locale_to_utf8("客户端",-1,NULL,NULL,NULL));
	table = gtk_table_new(2,2,FALSE);
	gtk_container_add(GTK_CONTAINER (user_window),table);
	label1 = gtk_label_new(g_locale_to_utf8("当前用户：",-1,NULL,NULL,NULL));
	label2 = gtk_label_new(gtk_entry_get_text(GTK_ENTRY(entry1)));
	gtk_table_attach (GTK_TABLE(table),label1,0,1,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),label2,1,2,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
    g_signal_connect(G_OBJECT(user_window),"delete_event",G_CALLBACK(gtk_main_quit),NULL);
    vbox = gtk_vbox_new(FALSE, 0);
    clist_1 = gtk_clist_new_with_titles(5, titles);
    gtk_box_pack_start(GTK_BOX(vbox), clist_1, TRUE, TRUE, 5);	
	gtk_table_attach (GTK_TABLE(table),vbox,0,2,1,2,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_widget_set_size_request(vbox,800,500);
	gtk_widget_set_size_request(label1,100,50);
	bbox = gtk_hbutton_box_new();
    button_tips = gtk_tooltips_new();
    gtk_box_pack_start(GTK_BOX(vbox), bbox, FALSE, FALSE, 5);
    gtk_box_set_spacing(GTK_BOX(bbox), 5);
	gtk_button_box_set_child_size(GTK_BUTTON_BOX(bbox), 70, 30);
	/*gtk_button_box_set_layout(GTK_BUTTON_BOX(bbox), GTK_BUTTONBOX_END);
    gtk_button_box_set_child_size(GTK_BUTTON_BOX(bbox), 20, 20);*/
    button = create_button(GTK_STOCK_SAVE);
    gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips), button, "下载到本地", NULL);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(goto_first), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 2);
    button = create_button(GTK_STOCK_DELETE);
    gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips), button, "将文件从库中移出", NULL);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(goto_back), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 2);
    button = create_button(GTK_STOCK_NEW);
    gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips), button, "添加文件", NULL);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(append_row), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 2);
    button = create_button(GTK_STOCK_FIND);
    gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips), button, "查询", NULL);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(goto_last), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 2);
    button = create_button(GTK_STOCK_QUIT);
    gtk_tooltips_set_tip(GTK_TOOLTIPS(button_tips), button, "退出", NULL);
    g_signal_connect(G_OBJECT(button), "clicked", G_CALLBACK(gtk_main_quit), NULL);
    gtk_box_pack_start(GTK_BOX(bbox), button, FALSE, FALSE, 5);
	
	gtk_window_set_position( GTK_WINDOW(user_window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_widget_show_all (user_window);
}	

void create_main_window()	//登录界面
{
	GtkWidget *table;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *label3;
	GtkWidget *label4;
	GtkWidget *button;
	
	main_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(main_window),g_locale_to_utf8("账户登录",-1,NULL,NULL,NULL));
	table = gtk_table_new(5,3,TRUE);
	label1 = gtk_label_new(g_locale_to_utf8("账户:",-1,NULL,NULL,NULL));
	label2 = gtk_label_new(g_locale_to_utf8("密码:",-1,NULL,NULL,NULL));
	label3 = gtk_label_new(g_locale_to_utf8("服务器IP:",-1,NULL,NULL,NULL));
	label4 = gtk_label_new(g_locale_to_utf8("端口:",-1,NULL,NULL,NULL));
	entry1 = gtk_entry_new();
	entry2 = gtk_entry_new();
	entry_ip = gtk_entry_new();
	entry_port = gtk_entry_new();
	button = gtk_button_new_with_label(g_locale_to_utf8("登录",-1,NULL,NULL,NULL));
	gtk_container_add(GTK_CONTAINER (main_window),table);
	
	gtk_table_attach (GTK_TABLE(table),label3,0,1,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),entry_ip,1,3,0,1,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),label4,0,1,1,2,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),entry_port,1,2,1,2,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	//gtk_entry_set_position(GTK_ENTRY(entry_port),GTK_JUSTIFY_LEFT);
	gtk_table_attach (GTK_TABLE(table),label1,0,1,2,3,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),label2,0,1,3,4,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),entry1,1,3,2,3,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),entry2,1,3,3,4,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	gtk_table_attach (GTK_TABLE(table),button,1,2,4,5,(GtkAttachOptions)(0),(GtkAttachOptions)(0),10,10);
	g_signal_connect (G_OBJECT(button),"clicked",G_CALLBACK(on_clicked),main_window);
	//gtk_widget_set_size_request(entry_ip,100,30);
	gtk_widget_set_size_request(entry_port,50,30);
	//gtk_widget_set_usize(main_window,1000,300);
	gtk_window_set_position( GTK_WINDOW(main_window), GTK_WIN_POS_CENTER_ALWAYS );
	gtk_widget_show_all (main_window);
	//return main_window;
}


//void  gtk_label_set_text (GtkLabel *label,const gchar *str);
//const gchar *gtk_label_get_text( GtkLabel *label );
//gtk_widget_destroy()


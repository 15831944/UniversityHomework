#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<gtk/gtk.h>

#define SERVER_PORT 8000
#define BUF_SIZE 1024
void deal_pressed1(GtkButton *button, gpointer user_data) //start
{
	int clientSock = -1;
	struct sockaddr_in client_addr ,server_addr;
	socklen_t server_addr_length;
   	char buffer[BUF_SIZE];
	int length = 0;
	FILE *fp;

	//��ʼ��һ���ͻ��˵�socket��ַ�ṹ
	bzero(&client_addr, sizeof(client_addr));
	client_addr.sin_family = AF_INET;
	client_addr.sin_addr.s_addr = htons(INADDR_ANY);
	client_addr.sin_port = htons(0);

	//��ʼ��һ���������˵�socket��ַ�ṹ
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(SERVER_PORT);
	server_addr_length = sizeof(server_addr);

	//����socket�����ɹ�������socket������
	clientSock = socket(AF_INET, SOCK_STREAM, 0);
	if(clientSock < 0)
	{
		perror("Create Socket Failed:");
		exit(1);
	}
	printf("Creat Socket Success.\n");

	//�󶨿ͻ��˵�socket�Ϳͻ��˵�socket��ַ�ṹ
	if((bind(clientSock, (struct sockaddr*)&client_addr, sizeof(client_addr))) == -1)
	{
		perror("Client Bind Error:");
		exit(1);
	}
	printf("Client Bind Success.\n");


	// ���������������
	if(connect(clientSock, (struct sockaddr*)&server_addr, server_addr_length) < 0)
	{
		perror("Connect error:");
		exit(1);
	}
	printf("Connect Success.\n");

		// ���ļ�����ȡ�ļ�����
		fp = fopen("Packet.txt", "r");
		if(NULL == fp)
		{
			printf("File Not Found\n");
		}
		else
		{
			printf("File has opened successfully.\n");
			bzero(buffer, BUF_SIZE);

			// ÿ��ȡһ�����ݣ��㽫�䷢�͸��ͻ��ˣ�ѭ��ֱ���ļ�����Ϊֹ
			while(fgets(buffer, BUF_SIZE, fp) != NULL)
			{
				printf("buffer:%s ", buffer);
				length = send(clientSock, buffer, sizeof(buffer), 0);
				printf("length:%d\n",length);
				if(length <0)
				{
					perror("Send Failed:");
					break;
				}else{

					printf("Send success.");
				}
				bzero(buffer, BUF_SIZE);
			}
			// �ر��ļ�
			fclose(fp);
			//printf("File Transfer Successful!\n");
		}

	close(clientSock);
	return 0;
}
void deal_pressed2(GtkButton *button, gpointer user_data) //end 
{
return 0;
}
int main(int argc,char *argv[])
{
	//1.gtk�����ĳ�ʼ��
    gtk_init(&argc, &argv);
    //2.����һ������
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *table = gtk_table_new(3,5,TRUE);
     //��table���뵽window��
     gtk_container_add(GTK_CONTAINER(window), table);



         //3.����һ����ťbutton
        GtkWidget *button1 = gtk_button_new_with_label("Start");
  GtkWidget *button2 = gtk_button_new_with_label("End");

     gtk_table_attach_defaults(GTK_TABLE(table), button1, 1, 2, 1, 2);
     gtk_table_attach_defaults(GTK_TABLE(table), button2, 3, 4, 1, 2);
   

        //4.����һ����ť�ź�
        g_signal_connect(button1, "pressed", G_CALLBACK(deal_pressed1), " Start");  //start
        g_signal_connect(button1, "pressed", G_CALLBACK(deal_pressed2), " End"); //end

        //5.��ʾ���ؼ�
        gtk_widget_show_all(window);

        //6.���¼�ѭ��
        gtk_main();

        return 0;
}

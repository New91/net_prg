#include "EasyTcpClient.hpp"
#include <thread>

void cmdThread(EasyTcpClient* client)
{
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit")) {
			client->Close();
			printf("退出cmdThread线程\n");
			break;
		}
		else if (0 == strcmp(cmdBuf, "login")) {
			Login login;
			strcpy(login.userName, "tom");
			strcpy(login.PassWord, "tom123");
			client->SendData(&login);
		}
		else if (0 == strcmp(cmdBuf, "logout")) {
			Logout logout;
			strcpy(logout.userName, "tom");
			client->SendData(&logout);
		}
		else {
			printf("unkonw。\n");
		}
	}
}
int main()
{
	EasyTcpClient client;
	const char* ip = "127.0.0.1";
	client.Connect(ip, 4567);

	//启动UI线程
	std::thread t1(cmdThread, &client);
	t1.detach();

	while (client.isRun()) {
		client.OnRun();
	}
	client.Close();
	printf("已退出....");
	getchar();
	return 0;
}
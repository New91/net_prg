#pragma once

#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS

#include<windows.h>
#include<WinSock2.h>
#include<stdio.h>

#pragma comment(lib,"ws2_32.lib")
#include "MessageHeader.hpp"

class EasyTcpClient
{
	SOCKET _sock;
public:
	EasyTcpClient() {
		_sock = INVALID_SOCKET;
	}

	virtual ~EasyTcpClient()
	{
		Close();
	}

	// ��ʼ��socket
	void InitSocket(){
		WORD ver = MAKEWORD(2, 2);
		WSADATA dat;
		WSAStartup(ver, &dat);

		if (INVALID_SOCKET != _sock) {
			printf("<socket=%d>�رվ�����...\n", _sock);
			Close();
		}
		_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if(INVALID_SOCKET == _sock)
		{
			printf("���󣬽���Socketʧ��...\n");
		}
		else {
			printf("����Socket�ɹ�...\n");
		}
	}

	// ���ӷ�����
	int Connect(const char *ip, unsigned short port)
	{
		if (INVALID_SOCKET == _sock) {
			InitSocket();
		}

		sockaddr_in _sin = {};
		_sin.sin_family = AF_INET;
		_sin.sin_port = htons(port);
		_sin.sin_addr.S_un.S_addr = inet_addr(ip);

		int ret = connect(_sock, (sockaddr*)&_sin, sizeof(sockaddr_in));
		if (SOCKET_ERROR == ret)
		{
			printf("�������ӷ�����ʧ��...\n");
		}
		else {
			printf("���ӷ������ɹ�...\n");
		}
		return ret;
	}
	// �ر��׽���
	void Close()
	{
		if (_sock != INVALID_SOCKET) {
			closesocket(_sock);
			WSACleanup();
			_sock = INVALID_SOCKET;
		}
	}
	// ����������Ϣ
	bool OnRun() {
		if (isRun()) {
			fd_set fdReads;
			FD_ZERO(&fdReads);
			FD_SET(_sock, &fdReads);
			timeval t = { 1,0 };
			int ret = select(_sock + 1, &fdReads, 0, 0, &t);
			if (ret < 0) {
				printf("<socket=%d>select�������1\n", _sock);
				return false;
			}
			if (FD_ISSET(_sock, &fdReads))
			{
				FD_CLR(_sock, &fdReads);
				if (-1 == RecvData(_sock)) {
					printf("<socket=%d>select�������2\n", _sock);
					return false;
				}
			}
			return true;
		}
		return false;
	}

	//�Ƿ�����
	bool isRun() {
		return _sock != INVALID_SOCKET;
	}

	//�������� ����ճ�� ��ְ�
	int RecvData(SOCKET _cSock)
	{
		// ������
		char szRecv[4096] = {};
		// ���ܿͻ�������
		int nLen = (int)recv(_sock, szRecv, sizeof(DataHeader), 0);
		DataHeader* header = (DataHeader*)szRecv;
		if(nLen <= 0) {
			printf("��������Ͽ����ӣ����������\n");
			return -1;
		}
		recv(_sock, szRecv + sizeof(DataHeader), header->dataLength - sizeof(DataHeader),0);
		OnNetMsg(header);
		return 0;
	}

	// ��Ӧ������Ϣ
	void OnNetMsg(DataHeader* header) {
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{

			LoginResult* login = (LoginResult*)header;
			printf("�յ��������Ϣ��CMD_LOGIN_RESULT,���ݳ��ȣ�%d\n", login->dataLength);
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			LogoutResult* logout = (LogoutResult*)header;
			printf("�յ��������Ϣ��CMD_LOGOUT_RESULT,���ݳ��ȣ�%d\n", logout->dataLength);
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			NewUserJoin* userJoin = (NewUserJoin*)header;
			printf("�յ��������Ϣ��CMD_NEW_USER_JOIN,���ݳ��ȣ�%d\n", userJoin->dataLength);
		}
		break;
		}
	}

	// ������Ϣ
	int SendData(DataHeader* header)
	{
		if (isRun() && header) {
			return send(_sock, (const char*)header, header->dataLength, 0);
		}
		return SOCKET_ERROR;
	}
};
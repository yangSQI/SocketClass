#ifndef __NETEVENTSERVER_H__
#define __NETEVENTSERVER_H__
#include "../SocketClass/NetEvent.hpp"
#include "../SocketClass/DataGather.hpp"
namespace yang
{
	class NetEventServer : public NetEvent
	{
	public:
		/**
		* @description : �������������Ϣ
		* @param : _sockInfo: sock��Ϣ�ṹ��
		*/
		void OnNetRecv(SocketInfo* _sockInfo)
		{
			char* startPos = _sockInfo->_recvStart;
			char* lastPos = _sockInfo->_recvLast;
			char* buff = startPos;
			DataHeader* lpDataHeader = NULL;
			// ճ�����ٰ�����	
			int len = 0;
			while ((len = lastPos - startPos) > sizeof(DataHeader))
			{
				lpDataHeader = (DataHeader*)startPos;
				if (len > lpDataHeader->len)
				{
					//printf("SOCKET: %d\t", sockInfo->_sock);
					switch (lpDataHeader->dataType)
					{
					case DataType::LOGIN:
					{
						Login* lpLogin = (Login*)startPos;
						printf("�û���: %s, ����: %s, ���ݳ���: %d\n", lpLogin->username, lpLogin->password, lpLogin->dataHeader.len);
						if (_sockInfo->_sendLen - (_sockInfo->_sendLast - _sockInfo->_sendStart) >= sizeof(LoginReply))
						{
							// �ظ���Ϣ
							LoginReply loginReply;
							snprintf(loginReply.reply, 100, "�յ������Ϣ��, �û���: %s, ����: %s", lpLogin->username, lpLogin->password);
							memmove(_sockInfo->_sendLast, (char*)&loginReply, sizeof(LoginReply));
							_sockInfo->_sendLast += sizeof(LoginReply); // ���ͻ�����βλ��
						}
						startPos += lpLogin->dataHeader.len;
						break;
					}
					case DataType::LOGOUT:
					{
						Logout* lpLogout = (Logout*)startPos;
						printf("�û�id: %u, ���ݳ���: %d\n", lpLogout->uid, lpLogout->dataHeader.len);
						if (_sockInfo->_sendLen - (_sockInfo->_sendLast - _sockInfo->_sendStart) >= sizeof(LogoutReply))
						{
							// �ظ���Ϣ
							LoginReply logoutReply;
							snprintf(logoutReply.reply, 100, "�յ������Ϣ��, �û�id: %d", lpLogout->uid);
							memmove(_sockInfo->_sendLast, (char*)&logoutReply, sizeof(LogoutReply));
							_sockInfo->_sendLast += sizeof(LoginReply); // ���ͻ�����βλ��
						}
						startPos += lpLogout->dataHeader.len;
						break;
					}
					}
				}
				else
				{
					break;
				}
			}
			if (buff != startPos)
			{
				//printf("ʣ�೤��%d\n", len);
				memcpy(buff, startPos, len);
			}
			// ʣ��λ��
			_sockInfo->_recvLast = buff + len;
			// ����sock���ͻ�������ӽ�������Ϣ�м���б���
			SocketHandle::send_push_back(_sockInfo);
		}
		/**
		* @description : ����ͻ����뿪��Ϣ
		* @param : _sockInfo: sock��Ϣ�ṹ��
		*/
		void OnNetLeave(SocketInfo* _sockInfo)
		{
			--_sockNum;
			#ifdef _WIN32
				char buff[20];
				snprintf(buff, 20, "SockNum: %d", _sockNum);
				SetConsoleTitleA(buff);
			#endif
			//printf("socket<%d> leval, port: %d, ip: %s\n", _sockInfo->_sock, ntohs(_sockInfo->_sockaddr.sin_port), inet_ntoa(_sockInfo->_sockaddr.sin_addr));
		}
		/**
		* @description : �ͻ��˵�����Ϣ
		* @param : _sockInfo: sock��Ϣ�ṹ��
		*/
		void OnNetAccept(SocketInfo* _sockInfo)
		{
			++_sockNum;
			#ifdef _WIN32
				char buff[20];
				snprintf(buff, 20, "SockNum: %d", _sockNum);
				SetConsoleTitleA(buff);
			#endif
			//printf("�ͻ���SOCKET����: %d, �˿�: %d, ip: %s\n", _sockInfo->_sock, ntohs(_sockInfo->_sockaddr.sin_port), inet_ntoa(_sockInfo->_sockaddr.sin_addr));
		}
	};
};
#endif
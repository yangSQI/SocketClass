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
		* @description : 处理网络接收消息
		* @param : _sockInfo: sock信息结构体
		*/
		void OnNetRecv(SocketInfo* _sockInfo)
		{
			char* startPos = _sockInfo->_recvStart;
			char* lastPos = _sockInfo->_recvLast;
			char* buff = startPos;
			DataHeader* lpDataHeader = NULL;
			// 粘包，少包处理	
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
						printf("用户名: %s, 密码: %s, 数据长度: %d\n", lpLogin->username, lpLogin->password, lpLogin->dataHeader.len);
						if (_sockInfo->_sendLen - (_sockInfo->_sendLast - _sockInfo->_sendStart) >= sizeof(LoginReply))
						{
							// 回复消息
							LoginReply loginReply;
							snprintf(loginReply.reply, 100, "收到你的消息啦, 用户名: %s, 密码: %s", lpLogin->username, lpLogin->password);
							memmove(_sockInfo->_sendLast, (char*)&loginReply, sizeof(LoginReply));
							_sockInfo->_sendLast += sizeof(LoginReply); // 发送缓冲区尾位置
						}
						startPos += lpLogin->dataHeader.len;
						break;
					}
					case DataType::LOGOUT:
					{
						Logout* lpLogout = (Logout*)startPos;
						printf("用户id: %u, 数据长度: %d\n", lpLogout->uid, lpLogout->dataHeader.len);
						if (_sockInfo->_sendLen - (_sockInfo->_sendLast - _sockInfo->_sendStart) >= sizeof(LogoutReply))
						{
							// 回复消息
							LoginReply logoutReply;
							snprintf(logoutReply.reply, 100, "收到你的消息啦, 用户id: %d", lpLogout->uid);
							memmove(_sockInfo->_sendLast, (char*)&logoutReply, sizeof(LogoutReply));
							_sockInfo->_sendLast += sizeof(LoginReply); // 发送缓冲区尾位置
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
				//printf("剩余长度%d\n", len);
				memcpy(buff, startPos, len);
			}
			// 剩余位置
			_sockInfo->_recvLast = buff + len;
			// 将该sock发送缓冲区添加进发送消息中间层列表中
			SocketHandle::send_push_back(_sockInfo);
		}
		/**
		* @description : 处理客户端离开消息
		* @param : _sockInfo: sock信息结构体
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
		* @description : 客户端到来消息
		* @param : _sockInfo: sock信息结构体
		*/
		void OnNetAccept(SocketInfo* _sockInfo)
		{
			++_sockNum;
			#ifdef _WIN32
				char buff[20];
				snprintf(buff, 20, "SockNum: %d", _sockNum);
				SetConsoleTitleA(buff);
			#endif
			//printf("客户端SOCKET接入: %d, 端口: %d, ip: %s\n", _sockInfo->_sock, ntohs(_sockInfo->_sockaddr.sin_port), inet_ntoa(_sockInfo->_sockaddr.sin_addr));
		}
	};
};
#endif
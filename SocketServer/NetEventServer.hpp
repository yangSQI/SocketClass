#ifndef __NETEVENTSERVER_H__
#define __NETEVENTSERVER_H__
#include "../SocketClass/NetEvent.hpp"
#include "../SocketClass/DataGather.hpp"
#include <atomic>
extern std::atomic_uint packageNum;
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
						++packageNum;
						Login* lpLogin = (Login*)startPos;
						printf("用户名: %s, 密码: %s, 数据长度: %d\n", lpLogin->username, lpLogin->password, lpLogin->dataHeader.len);
						startPos += lpLogin->dataHeader.len;
						break;
					}
					case DataType::LOGOUT:
					{
						Logout* lpLogout = (Logout*)startPos;
						printf("用户id: %u, 数据长度: %d\n", lpLogout->uid, lpLogout->dataHeader.len);
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
		}
		/**
		* @description : 处理客户端离开消息
		* @param : _sockInfo: sock信息结构体
		*/
		void OnNetLeave(SocketInfo* _sockInfo)
		{
			printf("socket<%d> leval, port: %d, ip: %s\n", _sockInfo->_sock, ntohs(_sockInfo->_sockaddr.sin_port), inet_ntoa(_sockInfo->_sockaddr.sin_addr));
		}
	};
};
#endif
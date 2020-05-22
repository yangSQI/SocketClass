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
						++packageNum;
						Login* lpLogin = (Login*)startPos;
						printf("�û���: %s, ����: %s, ���ݳ���: %d\n", lpLogin->username, lpLogin->password, lpLogin->dataHeader.len);
						startPos += lpLogin->dataHeader.len;
						break;
					}
					case DataType::LOGOUT:
					{
						Logout* lpLogout = (Logout*)startPos;
						printf("�û�id: %u, ���ݳ���: %d\n", lpLogout->uid, lpLogout->dataHeader.len);
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
		}
		/**
		* @description : ����ͻ����뿪��Ϣ
		* @param : _sockInfo: sock��Ϣ�ṹ��
		*/
		void OnNetLeave(SocketInfo* _sockInfo)
		{
			printf("socket<%d> leval, port: %d, ip: %s\n", _sockInfo->_sock, ntohs(_sockInfo->_sockaddr.sin_port), inet_ntoa(_sockInfo->_sockaddr.sin_addr));
		}
	};
};
#endif
/*
 * lftp - file transfer program
 *
 * Copyright (c) 2000 by Alexander V. Lukyanov (lav@yars.free.net)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Id$ */

#ifndef FISH_H
#define FISH_H

#include "NetAccess.h"
#include "StatusLine.h"
/*#include "buffer.h"*/

class Fish : public NetAccess
{
   enum state_t
   {
      DISCONNECTED,
      CONNECTING,
      CONNECTED,
      FILE_RECV,
      FILE_SEND,
      WAITING,
      DONE
   };

   state_t state;

   void Init();

   int max_send;
   void	 Send(const char *format,...) PRINTF_LIKE(2,3);
   void	 SendMethod();

   IOBuffer *send_buf;
   IOBuffer *recv_buf;

   OutputFilter *filter_out;  // used in connecting
   int pipe_in[2];	      // used in connecting

   void Disconnect();
   int IsConnected()
      {
	 if(state==DISCONNECTED)
	    return 0;
	 if(state==CONNECTING)
	    return 1;
	 return 2;
      }

   off_t body_size;
   off_t bytes_received;

   enum expect_t
   {
      EXPECT_FISH,
      EXPECT_VER,
      EXPECT_PWD,
      EXPECT_CWD,
      EXPECT_DIR,
      EXPECT_RETR_SIZE,
      EXPECT_RETR
   };

   void PushExpect(expect_t);
   int HandleReplies();
   int ReplyLogPriority(int);

   expect_t *RespQueue;
   int	 RQ_alloc;   // memory allocated
   int	 RQ_head;
   int	 RQ_tail;

   char  **path_queue;
   int	 path_queue_len;
   void  PushDirectory(const char *);
   char  *PopDirectory();

   int   RespQueueIsEmpty() { return RQ_head==RQ_tail; }
   int	 RespQueueSize() { return RQ_tail-RQ_head; }
   void  EmptyRespQueue() { RQ_head=RQ_tail=0; }

   char  *line;
   char  *message;

   bool	 eof;

   const char *shell_encode(const char *);

public:
   static void ClassInit();

   Fish();
   Fish(const Fish*);
   ~Fish();

   const char *GetProto() { return "fish"; }

   FileAccess *Clone() { return new Fish(this); }
   static FileAccess *New();

   int Do();
   int Done();
   int Read(void *,int);
   int Write(const void *,int);
   int StoreStatus();
   int SendEOT();

   void Close();
   const char *CurrentStatus();

   void Reconfig(const char *name=0);

   bool SameSiteAs(FileAccess *fa);
   bool SameLocationAs(FileAccess *fa);
};

#endif

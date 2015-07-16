/*
 * ac_test.cpp

 *
 *  Created on: Nov 26, 2014
 *      Author: leo
 */
#include <iostream>
//#include "Aris_ControlData.h"
#include "Aris_Control.h"
#include "Aris_Message.h"
#include "Aris_Thread.h"
#include "Aris_Socket.h"
#include "Server.h"

using namespace std;
using namespace Aris::RT_CONTROL;

static CGait gait;
static EGAIT gaitcmd[AXIS_NUMBER];
static EGAIT gaitcmdtemp[AXIS_NUMBER];

Aris::RT_CONTROL::ACTUATION cs;
//CSysBase sysbase;
Aris::RT_CONTROL::CSysInitParameters initParam;

 enum MACHINE_CMD
 {
     NOCMD=1000,
	 POWEROFF=1001,
	 STOP=1002,
	 ENABLE=1003,
	 RUNNING=1004,
	 GOHOME_1=1005,
	 GOHOME_2=1006,
	 HOME2START_1=1007,
	 HOME2START_2=1008,
 	 FORWARD=1009,
	 BACKWARD=1010,
	 FAST_FORWARD=1011,
	 FAST_BACKWARD=1012,
	 LEGUP=1013,
	 TURNLEFT=1014,
	 TURNRIGHT=1015,
	 ONLINEGAIT=1016
 };

 int count;

/*
 * Trajectory Generator
 */

 void* Thread2(void *)
 {
	 cout<<"running msgloop"<<endl;
 	Aris::Core::RunMsgLoop();
 	return NULL;
 };


int initFun(Aris::RT_CONTROL::CSysInitParameters& param)
{
	gait.InitGait(param);
	return 0;
};

int tg(Aris::RT_CONTROL::CMachineData& machineData,Aris::Core::RT_MSG& msgRecv,Aris::Core::RT_MSG& msgSend)
{
	static int tg_count;
	tg_count++;
	if(tg_count%1000==0)
	{
 		    msgSend.SetMsgID(RT_Data_Received);

	    	msgSend.SetLength(sizeof(int)*18*5);
	    	//states , mode, pos, vel, current

	    	for(int i=0;i<18;i++)
	    	{
	    		msgSend.CopyAt(&machineData.motorsStates[i],sizeof(int),i*5);
	    		msgSend.CopyAt(&machineData.motorsModesDisplay[i],sizeof(int),i*5+1);
		    	msgSend.CopyAt(&machineData.feedbackData[0].Position,sizeof(int),i*5+2);
		    	msgSend.CopyAt(&machineData.feedbackData[0].Position,sizeof(int),i*5+3);
		    	msgSend.CopyAt(&machineData.feedbackData[0].Position,sizeof(int),i*5+4);
	    	}


		 //     rt_printf("ty give msg id %d data length %d \n",msgSend.GetMsgID(),msgSend.GetLength());
 		//  rt_machine_msg.SetMsgID(1000);
		//  rt_machine_msg.Copy(&machineData,sizeof(machineData));

	         cs.RT_PostMsg(msgSend);
	}
 
	const int MapAbsToPhy[18]=
	{
			10,	11,	9,
			12,	14,	13,
			17,	15,	16,
			6,	8,	7,
			3,	5,	4,
			0,	2,	1
	};
	const int MapPhyToAbs[18]=
	{
			15,	17,	16,
			12,	14,	13,
			9,	11,	10,
			2,	0,	1,
			3,	5,	4,
			7,	8,	6
	};

 	int CommandID;

	 CommandID=msgRecv.GetMsgID();
 	switch(CommandID)
	{
	case NOCMD:
		for(int i=0;i<18;i++)
		{
	    	machineData.motorsCommands[i]=EMCMD_NONE;

		}
		rt_printf("NONE Command Get in NRT\n" );

	break;

	case ENABLE:
		 for(int i=0;i<18;i++)
		 {
			machineData.motorsCommands[i]=EMCMD_ENABLE;
		 }
 		rt_printf("ENABLE Command Get in NRT\n" );

		break;
	case POWEROFF:
		 for(int i=0;i<18;i++)
		 {
			machineData.motorsCommands[i]=EMCMD_POWEROFF;
			gait.IfReadytoSetGait(false,i);
		 }
		rt_printf("POWEROFF Command Get in NRT\n" );

		break;
	case STOP:
		 for(int i=0;i<18;i++)
		{
			machineData.motorsCommands[i]=EMCMD_STOP;
 		}
 		 rt_printf("STOP Command Get in NRT\n" );

		break;
	case RUNNING:
	 	for(int i=0;i<18;i++)
	 	{
			machineData.motorsCommands[i]=EMCMD_RUNNING;
			gait.IfReadytoSetGait(true,i);
	 	}
		rt_printf("RUNNING Command Get in NRT\n" );
		break;

	case GOHOME_1:

			machineData.motorsCommands[MapAbsToPhy[0]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[1]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[2]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[6]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[7]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[8]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[12]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[13]]=EMCMD_GOHOME;
			machineData.motorsCommands[MapAbsToPhy[14]]=EMCMD_GOHOME;

 			gaitcmd[MapAbsToPhy[0]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[1]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[2]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[6]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[7]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[8]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[12]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[13]]=EGAIT::GAIT_HOME;
 			gaitcmd[MapAbsToPhy[14]]=EGAIT::GAIT_HOME;

		rt_printf("GOHOME_1 Command Get in NRT\n" );

		break;

	case GOHOME_2:

		machineData.motorsCommands[MapAbsToPhy[3]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[4]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[5]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[9]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[10]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[11]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[15]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[16]]=EMCMD_GOHOME;
		machineData.motorsCommands[MapAbsToPhy[17]]=EMCMD_GOHOME;

			gaitcmd[MapAbsToPhy[3]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[4]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[5]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[9]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[10]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[11]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[15]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[16]]=EGAIT::GAIT_HOME;
			gaitcmd[MapAbsToPhy[17]]=EGAIT::GAIT_HOME;


		rt_printf("GOHOME_2 Command Get in NRT\n" );

		break;

	case HOME2START_1:

		if(gait.m_gaitState[MapAbsToPhy[0]]==GAIT_STOP)
		{
		    for(int i=0;i<18;i++)
		    {
				machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
		    }
				gaitcmd[MapAbsToPhy[0]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[1]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[2]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[6]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[7]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[8]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[12]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[13]]=EGAIT::GAIT_HOME2START;
				gaitcmd[MapAbsToPhy[14]]=EGAIT::GAIT_HOME2START;

			rt_printf("HOME2START_1 Command Get in NRT\n" );

		}



		break;

	case HOME2START_2:

		if(gait.m_gaitState[MapAbsToPhy[3]]==GAIT_STOP)
		{
		    for(int i=0;i<18;i++)
		    {
				machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
		    }

			gaitcmd[MapAbsToPhy[3]]=EGAIT::GAIT_HOME2START;
			gaitcmd[MapAbsToPhy[4]]=EGAIT::GAIT_HOME2START;
			gaitcmd[MapAbsToPhy[5]]=EGAIT::GAIT_HOME2START;
			gaitcmd[MapAbsToPhy[9]]=EGAIT::GAIT_HOME2START;
			gaitcmd[MapAbsToPhy[10]]=EGAIT::GAIT_HOME2START;
			gaitcmd[MapAbsToPhy[11]]=EGAIT::GAIT_HOME2START;
			gaitcmd[MapAbsToPhy[15]]=EGAIT::GAIT_HOME2START;
	 		gaitcmd[MapAbsToPhy[16]]=EGAIT::GAIT_HOME2START;
			gaitcmd[MapAbsToPhy[17]]=EGAIT::GAIT_HOME2START;

			rt_printf("HOME2START_2 Command Get in NRT\n" );
		}

		break;

 	case FORWARD:
	    for(int i=0;i<18;i++)
		 {
 			machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
			gaitcmdtemp[i]=EGAIT::GAIT_MOVE;
			machineData.motorsCommands[i]=EMCMD_RUNNING;


		 if(gait.m_gaitState[i]!=GAIT_STOP)
		 {
     	   gait.Gait_iter[i]=gait.Gait_iter[i]+1;
		 }
		 else
		 {
				gaitcmd[i]=gaitcmdtemp[i];
				gait.Gait_iter[i]=1;
			}

		 }

	   // rt_printf("driver 0 gaitcmd:%d\n",gaitcmd[0]);


		break;
 	case BACKWARD:
	    for(int i=0;i<18;i++)
		 {
 			machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
			gaitcmdtemp[i]=EGAIT::GAIT_MOVE_BACK;
			machineData.motorsCommands[i]=EMCMD_RUNNING;


		 if(gait.m_gaitState[i]!=GAIT_STOP)
		 {
     	   gait.Gait_iter[i]=gait.Gait_iter[i]+1;
		 }
		 else
		 {
				gaitcmd[i]=gaitcmdtemp[i];
				gait.Gait_iter[i]=1;
			}

		 }

	   // rt_printf("driver 0 gaitcmd:%d\n",gaitcmd[0]);


		break;

 	case FAST_FORWARD:
	    for(int i=0;i<18;i++)
		 {
 			machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
			gaitcmdtemp[i]=EGAIT::GAIT_FAST_MOVE;
			machineData.motorsCommands[i]=EMCMD_RUNNING;


		 if(gait.m_gaitState[i]!=GAIT_STOP)
		 {
     	   gait.Gait_iter[i]=gait.Gait_iter[i]+1;
		 }
		 else
		 {
				gaitcmd[i]=gaitcmdtemp[i];
				gait.Gait_iter[i]=1;
			}

		 }

	   // rt_printf("driver 0 gaitcmd:%d\n",gaitcmd[0]);


		break;
 	case FAST_BACKWARD:
	    for(int i=0;i<18;i++)
		 {
 			machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
			gaitcmdtemp[i]=EGAIT::GAIT_FAST_MOVE_BACK;
			machineData.motorsCommands[i]=EMCMD_RUNNING;


		 if(gait.m_gaitState[i]!=GAIT_STOP)
		 {
     	   gait.Gait_iter[i]=gait.Gait_iter[i]+1;
		 }
		 else
		 {
				gaitcmd[i]=gaitcmdtemp[i];
				gait.Gait_iter[i]=1;
			}

		 }


		break;
	case LEGUP:

		if(gait.m_gaitState[MapAbsToPhy[0]]==GAIT_STOP)
		{
		    for(int i=0;i<18;i++)
		    {
				machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
				gaitcmd[MapAbsToPhy[i]]=EGAIT::GAIT_LEGUP;
				machineData.motorsCommands[i]=EMCMD_RUNNING;

		    }
		}
		break;
	case TURNLEFT:

		if(gait.m_gaitState[MapAbsToPhy[0]]==GAIT_STOP)
		{
		    for(int i=0;i<18;i++)
		    {
				machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
				gaitcmd[MapAbsToPhy[i]]=EGAIT::GAIT_TURN_LEFT;
				machineData.motorsCommands[i]=EMCMD_RUNNING;

		    }
		}
		break;
	case TURNRIGHT:

		if(gait.m_gaitState[MapAbsToPhy[0]]==GAIT_STOP)
		{
		    for(int i=0;i<18;i++)
		    {
				machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
				gaitcmd[MapAbsToPhy[i]]=EGAIT::GAIT_TURN_RIGHT;
				machineData.motorsCommands[i]=EMCMD_RUNNING;

		    }
		}
		break;
	case ONLINEGAIT:

		if(gait.m_gaitState[MapAbsToPhy[0]]==GAIT_STOP)
		{
		    for(int i=0;i<18;i++)
		    {
				machineData.motorsModes[i]=EOperationMode::OM_CYCLICVEL;
				gaitcmd[MapAbsToPhy[i]]=EGAIT::GAIT_ONLINE;
				machineData.motorsCommands[i]=EMCMD_RUNNING;
		    }
		}
		break;

	default:
		//DO NOTHING, CMD AND TRAJ WILL KEEP STILL
 		break;
	}
    // gait.IfReadytoSetGait(machineData.isMotorHomed[0]);
    // rt_printf("driver 0 gaitcmd:%d\n",gaitcmd[0]);

     gait.RunGait(gaitcmd,machineData);

return 0;

};

//offsets driver order
static int HEXBOT_HOME_OFFSETS_RESOLVER[18] =
{
		-15849882+349000,	 -16354509+349000,	 -16354509+349000,
		-15849882+349000,	 -16354509+349000,	 -16354509+349000,
		-15849882+349000,	 -16354509+349000,	 -16354509+349000,
		-16354509+349000,	 -15849882+349000,	 -16354509+349000,
		-15849882+349000,	 -16354509+349000,	 -16354509+349000,
		-16354509+349000,	 -16354509+349000,       -15849882+349000
};


int OnGetControlCommand(Aris::Core::MSG &msg)
{
    int CommandID;
    msg.Paste(&CommandID,sizeof(int));
    Aris::Core::MSG data;

    switch(CommandID)
    {
    case 1:
		data.SetMsgID(POWEROFF);
		cs.NRT_PostMsg(data);
		break;
    case 2:
    	data.SetMsgID(STOP);
		cs.NRT_PostMsg(data);
    	break;
    case 3:
    	data.SetMsgID(ENABLE);
		cs.NRT_PostMsg(data);
    	break;
    case 4:
    	data.SetMsgID(RUNNING);
		cs.NRT_PostMsg(data);
    	break;
    case 5:
    	data.SetMsgID(GOHOME_1);
		cs.NRT_PostMsg(data);
    	break;
    case 6:
    	data.SetMsgID(GOHOME_2);
		cs.NRT_PostMsg(data);
    	break;
    case 7:
    	data.SetMsgID(HOME2START_1);
		cs.NRT_PostMsg(data);
    	break;
    case 8:
    	data.SetMsgID(HOME2START_2);
		cs.NRT_PostMsg(data);
    	break;
    case 9:
    	data.SetMsgID(FORWARD);
		cs.NRT_PostMsg(data);
    	break;
    case 10:
    	data.SetMsgID(BACKWARD);
		cs.NRT_PostMsg(data);
    	break;
    case 11:
    	data.SetMsgID(FAST_FORWARD);
		cs.NRT_PostMsg(data);
    	break;
    case 12:
    	data.SetMsgID(FAST_BACKWARD);
		cs.NRT_PostMsg(data);
    	break;
    case 13:
    	data.SetMsgID(LEGUP);
		cs.NRT_PostMsg(data);
    	break;
    case 14:
    	data.SetMsgID(TURNLEFT);
		cs.NRT_PostMsg(data);
    	break;
    case 15:
    	data.SetMsgID(TURNRIGHT);
		cs.NRT_PostMsg(data);
    	break;
    case 16:
    	data.SetMsgID(ONLINEGAIT);
    	cs.NRT_PostMsg(data);
    	break;


    default:
		printf("Hi! I didn't get validate cmd\n");
        break;

    }
    return CommandID;

};


int On_RT_DataReceived(Aris::Core::MSG &data)
{
	if(Is_CS_Connected==true)
	{
	    printf("Sending data to client,data length: %d\n",data.GetLength());
		ControlSystem.SendData(data);
	}
}
 

int main(int argc, char** argv)
{ 

    Aris::Core::RegisterMsgCallback(CS_Connected,On_CS_Connected);
    Aris::Core::RegisterMsgCallback(CS_CMD_Received,On_CS_CMD_Received);
    Aris::Core::RegisterMsgCallback(CS_Lost,On_CS_Lost);
    Aris::Core::RegisterMsgCallback(GetControlCommand,OnGetControlCommand);
    Aris::Core::RegisterMsgCallback(RT_Data_Received,On_RT_DataReceived);

//   CONN call back
	/*设置所有CONN类型的回调函数*/
   ControlSystem.SetCallBackOnReceivedConnection(On_CS_ConnectionReceived);
    //VisualSys.SetCallBackOnReceivedConnection(OnConnectReceived_VS);

	ControlSystem.SetCallBackOnReceivedData(On_CS_DataReceived);
      //  VisualSys.SetCallBackOnReceivedData(OnSysDataReceived_VS);

    ControlSystem.SetCallBackOnLoseConnection(On_CS_ConnectionLost);
      //  VisualSys.SetCallBackOnLoseConnection(OnConnectionLost_VS);

    ControlSystem.StartServer("5690");
     // VisualSys.StartServer("5691");
	Aris::Core::THREAD T1,T2,T3 ;
 //	 T1.SetFunction(Thread1);
	 T2.SetFunction(Thread2);
  // cs.Load_XML_PrintMessage();
     T2.Start(0);

	cs.SetSysInitializer(initFun);


	cs.SetTrajectoryGenerator(tg);

	//cs.SetModeCycVel();


	initParam.motorNum=18;
	initParam.homeHighSpeed=280000;
	initParam.homeLowSpeed=40000;

	initParam.homeMode=-1;
	initParam.homeTorqueLimit=100;
	////necessary steps
	initParam.homeOffsets=HEXBOT_HOME_OFFSETS_RESOLVER;
  	cs.SysInit(initParam);

	cs.SysInitCommunication();

	cs.SysStart();

	printf("Will start\n");
	while(!cs.IsSysStopped())
	{

		count++;
		sleep(1);
	}


	return 0;

};



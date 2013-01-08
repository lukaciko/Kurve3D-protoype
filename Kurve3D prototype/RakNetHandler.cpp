
#pragma once

#define GLFW_DLL

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include "RakNetHandler.h"
#include "NetworkIDManager.h"
#include "RakNetTime.h"
#include "GetTime.h"
#include "SocketLayer.h"

using namespace RakNet;

RakPeerInterface *rakPeer;
NetworkIDManager *networkIDManager;
ReplicaManager3Kurve3d *replicaManager3;
NatPunchthroughClient *natPunchthroughClient;
CloudClient *cloudClient;
RakNet::FullyConnectedMesh2 *fullyConnectedMesh2;
//PlayerReplica *playerReplica;

void InstantiateRakNetClasses(void)
{
	static const int MAX_PLAYERS=32;
	static const unsigned short TCP_PORT=0;
	static const RakNet::TimeMS UDP_SLEEP_TIMER=30;

	rakPeer=RakNet::RakPeerInterface::GetInstance();
	RakNet::SocketDescriptor sd(1234,0);
	sd.socketFamily=AF_INET; // Only IPV4 supports broadcast on 255.255.255.255
	while (SocketLayer::IsPortInUse(sd.port, sd.hostAddress, sd.socketFamily)==true)
		sd.port++;
	// +1 is for the connection to the NAT punchthrough server
	rakPeer->Startup(MAX_PLAYERS+1,&sd,1);
	rakPeer->SetMaximumIncomingConnections(MAX_PLAYERS);
	// Fast disconnect for easier testing of host migration
	rakPeer->SetTimeoutTime(5000,UNASSIGNED_SYSTEM_ADDRESS);
	// ReplicaManager3 replies on NetworkIDManager. It assigns numbers to objects so they can be looked up over the network
	// It's a class in case you wanted to have multiple worlds, then you could have multiple instances of NetworkIDManager
	networkIDManager=new NetworkIDManager;
}
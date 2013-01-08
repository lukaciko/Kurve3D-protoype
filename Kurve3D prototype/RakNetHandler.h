
#include "RakPeerInterface.h"
#include "ReplicaManager3.h"
#include "NatPunchthroughClient.h"
#include "CloudClient.h"
#include "FullyConnectedMesh2.h"
#include "UDPProxyClient.h"
#include "TCPInterface.h"
#include "HTTPConnection.h"
#include "MessageIdentifiers.h"

class ReplicaManager3Kurve3d;

extern RakNet::RakPeerInterface *rakPeer; // Basic communication
extern RakNet::NetworkIDManager *networkIDManager; // Unique IDs per network object
extern ReplicaManager3Kurve3d *replicaManager3; // Autoreplicate network objects
extern RakNet::NatPunchthroughClient *natPunchthroughClient; // Connect peer to peer through routers
extern RakNet::CloudClient *cloudClient; // Used to upload game instance to the cloud
extern RakNet::FullyConnectedMesh2 *fullyConnectedMesh2; // Used to find out who is the session host
//extern PlayerReplica *playerReplica; // Network object that represents the player


class BaseKurve3dReplica : public RakNet::Replica3
{
public:
	BaseKurve3dReplica();
	virtual ~BaseKurve3dReplica();
	virtual RakNet::RM3ConstructionState QueryConstruction(RakNet::Connection_RM3 *destinationConnection, RakNet::ReplicaManager3 *replicaManager3) {return QueryConstruction_PeerToPeer(destinationConnection);}
	virtual bool QueryRemoteConstruction(RakNet::Connection_RM3 *sourceConnection) {return QueryRemoteConstruction_PeerToPeer(sourceConnection);}
	virtual void DeallocReplica(RakNet::Connection_RM3 *sourceConnection) {delete this;}
	virtual RakNet::RM3QuerySerializationResult QuerySerialization(RakNet::Connection_RM3 *destinationConnection) {return QuerySerialization_PeerToPeer(destinationConnection);}
	virtual void SerializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *destinationConnection);
	virtual bool DeserializeConstruction(RakNet::BitStream *constructionBitstream, RakNet::Connection_RM3 *sourceConnection);
	virtual RakNet::RM3SerializationResult Serialize(RakNet::SerializeParameters *serializeParameters);
	virtual void Deserialize(RakNet::DeserializeParameters *deserializeParameters);
	virtual void SerializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *destinationConnection) {}
	virtual bool DeserializeDestruction(RakNet::BitStream *destructionBitstream, RakNet::Connection_RM3 *sourceConnection) {return true;}
	virtual RakNet::RM3ActionOnPopConnection QueryActionOnPopConnection(RakNet::Connection_RM3 *droppedConnection) const {return QueryActionOnPopConnection_PeerToPeer(droppedConnection);}

	/// This function is not derived from Replica3, it's specific to this app
	/// Called from CDemo::UpdateRakNet
	virtual void Update(RakNet::TimeMS curTime);

	// Set when the object is constructed	

	// real is written on the owner peer, read on the remote peer
	//irr::core::vector3df position;
	RakNet::TimeMS creationTime;
};
//class Connection_RM3Kurve3d : public RakNet::Connection_RM3 {
//public:
//	Connection_RM3Kurve3d(const RakNet::SystemAddress &_systemAddress, RakNet::RakNetGUID _guid, CDemo *_demo) : RakNet::Connection_RM3(_systemAddress, _guid) {demo=_demo;}
//	virtual ~Connection_RM3Kurve3d() {}
//
//	virtual RakNet::Replica3 *AllocReplica(RakNet::BitStream *allocationId, RakNet::ReplicaManager3 *replicaManager3);
//protected:
//	//CDemo *demo;
//};
//
//class ReplicaManager3Kurve : public RakNet::ReplicaManager3
//{
//public:
//	virtual RakNet::Connection_RM3* AllocConnection(const RakNet::SystemAddress &systemAddress, RakNet::RakNetGUID rakNetGUID) const {
//		return new Connection_RM3Kurve3d(systemAddress,rakNetGUID,demo);
//	}
//	virtual void DeallocConnection(RakNet::Connection_RM3 *connection) const {
//		delete connection;
//	}
//	//CDemo *demo;
//};
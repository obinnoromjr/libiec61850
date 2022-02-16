/*
 *  server_example_dynamic.c
 *
 *  This example shows how to build a data model at runtime by API calls.
 *
 */

#include "iec61850_server.h"
#include "hal_thread.h"
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

static int running = 0;

void sigint_handler(int signalId);
void LogicalNodeInfo(LogicalNode* lNode);
void create_ZBAT_LogicalNode(const char* node_name, LogicalDevice* device_name);
void create_ZBTC_LogicalNode(const char* node_name, LogicalDevice* device_name);
void create_ZINV_LogicalNode(const char* node_name, LogicalDevice* device_name);
void create_battery_instance(const char* zbat_name, const char* zbtc_name, const char* zinv_name, LogicalDevice* device_name);

int main(int argc, char** argv) {

    int tcpPort = 102;

    if (argc > 1) {
        tcpPort = atoi(argv[1]);
    }

    /*********************
     * Setup data model
     ********************/
	
	// Create model 
    IedModel* model = IedModel_create("testmodel");	
	
	// Create Battery System Logical Device
	LogicalDevice* Battery = LogicalDevice_create("Battery", model);
	
	// Create Logical node LLN0 (mandatory to include this node for IEC61850)
	LogicalNode* lln0 = LogicalNode_create("LLN0", Battery);
	LogicalNodeInfo(lln0);
	
	// create battery instance
	create_battery_instance("ZBAT", "ZBTC", "ZINV", Battery);
	
	/*********************
     * choose values
     ********************/
	 
	 LogicalNode* ZBAT = LogicalDevice_getLogicalNode(Battery, "ZBAT");
	 DataObject*  Vol  = (DataObject*) ModelNode_getChild( (ModelNode*) ZBAT, "Vol");

	 DataAttribute* voltageValue     = (DataAttribute*) ModelNode_getChild( (ModelNode*) Vol, "mag.f");
	 DataAttribute* voltageTimeStamp = (DataAttribute*) ModelNode_getChild( (ModelNode*) Vol, "t");

    /*********************
     * run server
     ********************/

	IedServer iedServer = IedServer_create(model);
	
	/* Set server ACCESS policies */
	IedServer_setWriteAccessPolicy(iedServer, IEC61850_FC_DC, ACCESS_POLICY_ALLOW);

	/* MMS server will be instructed to start listening to client connections. */
	IedServer_start(iedServer, tcpPort);

	if (!IedServer_isRunning(iedServer)) {
		printf("Starting server failed! Exit.\n");
		IedServer_destroy(iedServer);
		exit(-1);
	}

	running = 1;

	signal(SIGINT, sigint_handler);

	float val = 0.f;

	while (running) {
	    IedServer_lockDataModel(iedServer);

	    IedServer_updateUTCTimeAttributeValue(iedServer, voltageTimeStamp, Hal_getTimeInMs());
	    IedServer_updateFloatAttributeValue(iedServer, voltageValue, val);

	    IedServer_unlockDataModel(iedServer);

	    val += 0.1f;
		Thread_sleep(100);
	}

	/* stop MMS server - close TCP server socket and all client sockets */
	IedServer_stop(iedServer);

	/* Cleanup - free all resources */
	IedServer_destroy(iedServer);

	/* destroy dynamic data model */
	IedModel_destroy(model);
  return 0;
} 


void sigint_handler(int signalId)
{
	running = 0;
}

// Helper function to add mandatory params
// to all logical nodes 
void LogicalNodeInfo(LogicalNode* lNode) {
	CDC_INS_create("Mod",    (ModelNode*) lNode, 0);
	CDC_INS_create("Beh",    (ModelNode*) lNode, 0);
	CDC_INS_create("Health", (ModelNode*) lNode, 0);
	CDC_LPL_create("NamPlt", (ModelNode*) lNode, 0);
	return; 
}

void create_ZBAT_LogicalNode(const char* node_name, LogicalDevice* device_name) {
  // Add ZBAT Logical Node to Logical Device
	LogicalNode* ZBAT = LogicalNode_create(node_name, device_name);
	LogicalNodeInfo(ZBAT);

	// Add ZBAT Specific Data Objects
	CDC_SPS_create("BatSt",      (ModelNode *) ZBAT, 0);
	CDC_SPS_create("BatTestRsl", (ModelNode *) ZBAT, 0);
	CDC_SPS_create("BatVHi",     (ModelNode *) ZBAT, 0);
	CDC_SPS_create("BatVLo",     (ModelNode *) ZBAT, 0);

	CDC_ENG_create("BatTyp",     (ModelNode *) ZBAT, 0);
	CDC_ASG_create("AhrRtg",     (ModelNode *) ZBAT, 0, false);
	CDC_ASG_create("MinAhrRtg",  (ModelNode *) ZBAT, 0, false);
	CDC_ASG_create("BatVNom",    (ModelNode *) ZBAT, 0, false);
	CDC_ING_create("BatSerCnt",  (ModelNode *) ZBAT, 0);
	CDC_ING_create("BatParCnt",  (ModelNode *) ZBAT, 0);

	CDC_ASG_create("DisChaRte",  (ModelNode *) ZBAT, 0, false);
	CDC_ASG_create("MaxBatA",    (ModelNode *) ZBAT, 0, false);
	CDC_ASG_create("MaxChaV",    (ModelNode *) ZBAT, 0, false);
	CDC_ASG_create("HiBatVAlm",  (ModelNode *) ZBAT, 0, false);
	CDC_ASG_create("LoBatVAlm",  (ModelNode *) ZBAT, 0, false);

	CDC_MV_create("Vol",         (ModelNode *) ZBAT, 0, false);
	CDC_MV_create("VolChgRte",   (ModelNode *) ZBAT, 0, false);
	CDC_MV_create("InBatV",      (ModelNode *) ZBAT, 0, false);
	CDC_MV_create("Amp",         (ModelNode *) ZBAT, 0, false);
	CDC_MV_create("InBatA",      (ModelNode *) ZBAT, 0, false);
	CDC_MV_create("InBatTmp",    (ModelNode *) ZBAT, 0, false);	

	CDC_SPC_create("BatSt0",     (ModelNode *) ZBAT, 0, 0); // figure out what the control options are
	CDC_SPC_create("BatTest",    (ModelNode *) ZBAT, 0, 0);

	return;
}

void create_ZBTC_LogicalNode(const char* node_name, LogicalDevice* device_name) {
	// Add ZBTC Logical Node to Logical Device
	LogicalNode* ZBTC = LogicalNode_create(node_name, device_name);
	LogicalNodeInfo(ZBTC);

	// Add ZBTC Specific Data Objects
	CDC_ENG_create("BatChaSt",  (ModelNode *) ZBTC, 0);
	CDC_INS_create("ChaTms",    (ModelNode *) ZBTC, 0);

	CDC_ENG_create("BatChaTyp", (ModelNode *) ZBTC, 0);

	CDC_ASG_create("ReChaRte",  (ModelNode *) ZBTC, 0, false);
	CDC_ASG_create("BatChaPwr", (ModelNode *) ZBTC, 0, false);
	CDC_ENG_create("BatChaMod", (ModelNode *) ZBTC, 0);

	CDC_MV_create("ChaV",       (ModelNode *) ZBTC, 0, false);
	CDC_MV_create("ChaA",       (ModelNode *) ZBTC, 0, false);

	return;
}

void create_ZINV_LogicalNode(const char* node_name, LogicalDevice* device_name) {
	// Add ZINV Logical Node to Logical Device 
	LogicalNode* ZINV = LogicalNode_create(node_name, device_name);
	LogicalNodeInfo(ZINV); 
	
	CDC_ASG_create("WRtg",       (ModelNode *) ZINV, 0, false);
	CDC_ASG_create("VarRtg",     (ModelNode *) ZINV, 0, false);
	CDC_ENG_create("SwTyp",      (ModelNode *) ZINV, 0);
	CDC_ENG_create("CoolTyp",    (ModelNode *) ZINV, 0);
							     
	CDC_SPS_create("Stdby",      (ModelNode *) ZINV, 0);
	CDC_SPS_create("CurLev",     (ModelNode *) ZINV, 0);
	CDC_ENG_create("CmutTyp",    (ModelNode *) ZINV, 0);
	CDC_ENG_create("Isotyp",     (ModelNode *) ZINV, 0);
	CDC_ASG_create("SwHz",       (ModelNode *) ZINV, 0, false);
	CDC_ENG_create("GridMod",    (ModelNode *) ZINV, 0);
							     
	CDC_ENG_create("ACTyp",      (ModelNode *) ZINV, 0);
	CDC_ASG_create("OutWSet",    (ModelNode *) ZINV, 0, false); 
	CDC_ASG_create("OutVarSet",  (ModelNode *) ZINV, 0, false); 
	CDC_ASG_create("OutPFSet",   (ModelNode *) ZINV, 0, false); 
	CDC_ASG_create("OutHzSet",   (ModelNode *) ZINV, 0, false); 
	CDC_ASG_create("InALim",     (ModelNode *) ZINV, 0, false); 
	CDC_ASG_create("InVLim",     (ModelNode *) ZINV, 0, false); 
	CDC_ENG_create("PhACnfg",    (ModelNode *) ZINV, 0);
	CDC_ENG_create("PhBCnfg",    (ModelNode *) ZINV, 0);
	CDC_ENG_create("PhCCnfg",    (ModelNode *) ZINV, 0);
	
	CDC_MV_create("HeatSinkTmp", (ModelNode *) ZINV, 0, false);
	CDC_MV_create("EnclTmp",     (ModelNode *) ZINV, 0, false);
	CDC_MV_create("AmbAirTemp",  (ModelNode *) ZINV, 0, false);
	CDC_MV_create("FanSpdVal",   (ModelNode *) ZINV, 0, false);
	
	return;
}

void create_battery_instance(const char* zbat_name, const char* zbtc_name, const char* zinv_name, LogicalDevice* device_name) {
	create_ZBAT_LogicalNode(zbat_name, device_name);
	create_ZBTC_LogicalNode(zbtc_name, device_name);
	create_ZINV_LogicalNode(zinv_name, device_name);
	return;
}







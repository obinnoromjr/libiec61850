/*
 * client_example1.c
 *
 * This example is intended to be used with server_example_basic_io or server_example_goose.
 */

#include "iec61850_client.h"

#include <stdlib.h>
#include <stdio.h>

#include "hal_thread.h"

void closeConnection(IedConnection connection, bool error, char* hostname, int tcpPort) {
	if (error)
		printf("Failed to connect to %s:%i\n", hostname, tcpPort);
	
	IedConnection_close(connection);
	IedConnection_destroy(connection);
	return;
}

int main(int argc, char** argv) {

    char* hostname;
    int tcpPort = 102;

    if (argc > 1)
        hostname = argv[1];
    else
        hostname = "localhost";

    if (argc > 2)
        tcpPort = atoi(argv[2]);

    IedClientError error;

    IedConnection con = IedConnection_create();

    IedConnection_connect(con, &error, hostname, tcpPort);

    if (error == IED_ERROR_OK) {

        /* read an analog measurement value from server */
        MmsValue* value = IedConnection_readObject(con, &error, "testmodelBattery/ZBAT.Vol.mag.f", IEC61850_FC_MX);

        if (value != NULL) {

            if (MmsValue_getType(value) == MMS_FLOAT) {
                float fval = MmsValue_toFloat(value);
                printf("read float value: %f\n", fval);
            }
            else if (MmsValue_getType(value) == MMS_DATA_ACCESS_ERROR) {
                printf("Failed to read value (error code: %i)\n", MmsValue_getDataAccessError(value));
            }

            MmsValue_delete(value);
        }
		
        /* write a variable to the server */
		value = MmsValue_newVisibleString("NewBattery");
        IedConnection_writeObject(con, &error, "testmodelBattery/ZBAT.NamPlt.vendor", IEC61850_FC_DC, value);

        if (error != IED_ERROR_OK)
            printf("failed to write testmodelBattery/ZBAT.NamPlt.vendor (error code: %i)\n", error);
		
		// read variable that was just written to server
		MmsValue* secondValue = IedConnection_readObject(con, &error, "testmodelBattery/ZBAT.NamPlt.vendor", IEC61850_FC_DC);
		
		if (value != NULL) {
			
			if (MmsValue_getType(secondValue) == MMS_VISIBLE_STRING) {
				const char* sval = MmsValue_toString(secondValue);
				printf("read string value: %s\n", sval);
			}
		} else {
			printf("Error: The string is NULL\n");
		}

        MmsValue_delete(value);

        closeConnection(con, false, hostname, tcpPort);
	}
	return 0;
}



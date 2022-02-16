BATTERY_DIRS = client_iec61850
BATTERY_DIRS += server_iec61850

all: battery

battery:
	-for d in $(BATTERY_DIRS); do(cd $$d && $(MAKE)); done

clean:
	-for d in $(BATTERY_DIRS); do(cd $$d && $(MAKE) clean); done



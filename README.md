# DynamicSwap

Dynamic Re-Paging of Remote Memory Based on System Load.

# Hardware/OS Setup

Ensure that you have **two (2)** nodes with the following properties:

* (1+) Mellanox ConnectX-4+ NIC
* 256GB+ of SATA SSD Storage
* Ubuntu 20.04 (more recent versions show instability with Infiniswap)

*I found that the Xl170 node from CloudLab works well for this.*

# Startup

1. Ensure that you have an active reservation that meets the above hardware criteria.
2. Navigate to Cloudlab and select "Start Experiment" from the top-left dropdown.
3. Change the profile to "server-host-infiniswap" if you are using Xl170 nodes, or make a custom profile if not. If you do not see this profile, you can make your own under "My Profiles" -- see appendix A. Click next.
4. Give the experiment a unique name. Ensure that your reservation is active under the right "project." Click next.
5. Set the experiment duration as appropriate. Click Finish and wait for nodes to start.

# Machine Startup

### Server

1. SSH into the "Server" machine.
2. Download appropriate RDMA drivers from the link below. Use the most recent driver version and specify all other options as appropriate. The tutorial uses the .iso downloadable file:
* [RDMA DRIVERS](https://network.nvidia.com/products/infiniband-drivers/linux/mlnx_ofed/)
3. Mount the .iso on the machine to an appropriate location:

    `sudo mkdir -p /mnt/iso/ISwap`
    
    `sudo mount -o loop [PATH_TO_ISO] /mnt/iso/ISwap`

4. Install the drivers and confirm any prompts:

    `sudo /mnt/iso/ISwap/mlnxofedinstall`

5. Run any commands listed by the previous command to ensure successful driver installation and start. **NOTE: the machine may disconnect for a period of time (since network drivers are restarting) -- if this is longer than 2-ish minutes, perform a power-cycle and reconnect to the machine.**

6. Clone the Infiniswap project into the user home directory:

    `git clone https://github.com/SymbioticLab/Infiniswap.git`

7. Run `ifconfig` and take a note of the DHCP assigned IP address of the **Mellanox NIC Device.** Along with the **MASK** of the device.

8. Modify the `Infiniswap/setup/ib_setup.sh` script to reflect the mask from step 7. If my mask were (for example) `255.255.248.0` I would modify the CIDR net mask to `/21` on the fourth line of the file; e.g.:

    `sudo ifconfig ib0 $1/21`

9. Run the `ib_setup.sh` script with the IP from step 7 as the only argument. This should complete quickly and without interruption. If the device loses connection, your step 7 or 8 were done incorrectly:

    `cd setup`

    `sudo ./ib_setup.sh [IP_ADDR]`

10. Run the install script:

    `sudo ./install.sh daemon`

11. Run the daemon with the IP from step 7 and a high PORT number (e.g. 8000) to verify that the sever is fully functional; the command-line should print a listening indicator:

    `cd infiniswap_daemon`

    `./infiniswap_daemon [IP_ADDR] [PORT]`

### Client

1. Copy steps 1 - 9 from the server start-up listed above but with the "Client" node.

3. Change the portal list in `setup` to reflect the IP and port on which the server is listening:

    `cd setup`

    `nano portal.list`

        [IP_ADDR]:[PORT] # of server

2. Run the install script:

    `sudo ./install.sh bd`

3. Find any existing swap partitions:

    `sudo swapon -s`

5. Remove all swap partitions from step 3 with:

    `sudo swapoff [swap_partitions]`

6. Run block device setup:

    `sudo ./infiniswap_bd_setup.sh`

    *If there is an error, try:*

    ```
    cd infiniswap_bd
    make clean
    cd ../setup
    ./get_module.symvers.sh [mlnx_ofed_version]
    ```

    And restart from step 2.

7. The device is now ready for use in remote memory transactions -- note that applications must run in a container (e.g. LXC or Docker) to page memory out.

## Appendix A

Create your own profile by navigating to "My Profiles" and clicking on the profile creation link.

Ensure that you add two nodes, with unique names, which are connected via a mutual switch (you can drag from one node to the other).

Both nodes should be set to the same hardware (e.g. Xl170) and the same Ubuntu Linux version (e.g. 20.04). Both nodes can either operate on a bare-metal node or XenVM if you wish.


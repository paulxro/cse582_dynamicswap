# DynamicSwap

System-Aware Paging of Far Memory

# Hardware/OS Setup

Ensure that you have **two (2)** cloudlab XL170 nodes.

A profile named "server_host_AIFM" should be visible under the CSE582 cloudlab project profiles.

Start an experiment with both nodes running the same image. The topology of the experiment should reflect two nodes connected through the same switch.

# Startup

## Host

1. ssh into the "Host" machine and navigate to the "/local" directory. AIFM should be present.
2. Run `ifconfig` and copy the eno49 network interface IP address.
3. Modify "./AIFM/aifm/configs/server.config" (addr line) to reflect the ip from step (2).
4. Modify "./AIFM/aifm/configs/server.config" (netmask line) to "255.255.248.0".
5. Run `route -n` and copy the gateway for the 0.0.0.0 destination.
6. Modify "./AIFM/aifm/configs/server.config" (gateway line) to the gateway ip from step (5).
7. Run `sudo ./AIFM/aifm/build_all.sh`

## Client

1. Repeat steps 1 - 7 on the client machine (running all commands on the client again), but modifying "./AIFM/aifm/configs/client.config" instead.
2. Generate a new ssh key with `ssh-keygen -t ed25519 -C "client_node"`
3. Run `cat ~/.ssh/id_ed25519.pub` and copy the output.
4. Include this public key on the *HOST* machine under "~/.ssh/authorized_keys" file. This can be done via `sudo nano ~/.ssh/authorized_keys`.
5. Modify `./AIFM/aifm/configs/ssh` to use the new private key and SSH to the correct user. An example of a correct ssh file is as follows:

```
MEM_SERVER_SSH_IP=128.110.218.182 # IP of host machine
MEM_SERVER_SSH_USER=paldea # username of client

function ssh_execute {
    ssh $MEM_SERVER_SSH_USER@$MEM_SERVER_SSH_IP -o "IdentitiesOnly=yes" -i "~/.ssh/id_ed25519" $1
}

function ssh_execute_tty {
    ssh $MEM_SERVER_SSH_USER@$MEM_SERVER_SSH_IP -o "IdentitiesOnly=yes" -i "~/.ssh/id_ed25519" -t $1
}
```

6. Modify `./AIFM/aifm/shared.sh` to reflect the host machine's IP on line 6.
7. Run `sudo ./AIFM/build_all.sh` if not already done in previous steps.
8. Run `cd ./AIFM/aifm && ./test.sh`. All tests should pass; this may take a few minutes.

## DynamicSwap

1. Clone the DynamicSwap repository in the local folder: `cd /local && git clone https://github.com/paulxro/cse582_dynamicswap.git`
2. Enter new folder: `cd cse582_dynamicswap`
3. Copy ALL files in "./aifm_changes" to either "/local/AIFM/aifm/inc" or "/local/AIFM/aifm/src" or "/local/AIFM/aifm" based on file type. All cpp files should be placed in src, all hpp files should be placed in inc, and the Makefile should be placed in "aifm".
7. Run `rm /local/AIFM/aifm/bin/*`.
4. Run `make` and the DynamicSwap source should be built. If error occurs, you may need to manually create an obj folder under unit_tests and cse582_dynamicswap: `mkdir /local/cse582_dynamicswap/obj && mkdir /local/cse582_dynamicswap/unit_tests/obj`. If permission errors occur, make sure the current user has ownership privileges over the directory: `sudo chown -R $USER: /local`.
5. It is OK for warnings to be displayed, so long as compilation is successful.
8. Run `cd /local/AIFM/aifm`.
9. Run `./test.sh`. The first tcp test should pass. You may compile more tests by modifying the makefile found under "/local/cse582_dynamicswap/" on line 36 by appending the names of tests found under "/local/cse582_dynamicswap/unittests/src".

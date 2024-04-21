## Compiling KMS

To compile the KMS, use the following command:

```bash
g++ -o kms KMS.cpp etsi_qkd_004.cpp ETSI004_block.cpp ip_tunnel_ms_windows_20200819.cpp load_ascii_20200819.cpp message_handler.cpp ms_windows_console_output_common_20200819.cpp netxpto_20200819.cpp save_ascii_2024.cpp
```

## Compiling KeyProviderServer

To compile the KeyProviderServer, use the following command:

```bash
g++ -o key_provider keyProviderServer.cpp etsi_qkd_004.cpp ETSI004_block.cpp ip_tunnel_ms_windows_20200819.cpp load_ascii_20200819.cpp message_handler.cpp ms_windows_console_output_common_20200819.cpp netxpto_20200819.cpp save_ascii_2024.cpp
```

## Running the program

To run the program you need to run `./kms` and `./key_provider` in distinct terminals.

## Input Parameters

You can change the input parameters for KMS by editing `input_parameters_KMS.txt`.
You can change the input parameters for KeyProviderServer by editing `input_parameters_KeyProvider.txt`.

Each line in these files should contain a parameter name and its value, separated by an equals sign (`=`). For example:

```txt
parameter1=value1
parameter2=value2
```
Please note that these files should not contain any empty lines.
After changing the parameters, save the file and re-run (no need for re-compiling) the KMS or KeyProviderServer to use the new parameters.

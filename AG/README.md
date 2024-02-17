g++ -std=c++17 -g -o main main_app.cpp netxpto_20200819.cpp load_ascii_20200819.cpp console.cpp

A app já lê os ficheiros .dat da maneira correta.

Proximos passos:
- Ajeitar a leitura atraves de um diretorio
- Começar a criar tentar enviar os sinais de saida para um arquivo (ou diretamente para uma app)
-Implementar o message handler e o ip Tunnel de modo a termos a interface da Etsi

Starting program: /home/h1de/KMS/AG/main 
Iniciando...
i = 1

Breakpoint 1, SaveAscii::runBlock (this=0x7fffffffd620) at save_ascii_20200819.cpp:56
56		t_string asciiFileName_;
(gdb) print asciiFileName
$1 = "rx_raw"
(gdb) print endFile
$2 = 11664
(gdb) print outPosition
$3 = 0
(gdb) print inputSignals[0]->ready()
$4 = 0

Based on the output from GDB, it appears that the `SaveAscii::runBlock` method is indeed being called, and you've successfully inspected the critical variables within it. Here's a breakdown of what each of the printed values signifies:

- `asciiFileName` is `"rx_raw"`, indicating the base name for your output file is correctly set.
- `endFile` is `11664`, which seems to be a predefined or calculated value indicating either the total number of data points you expect to write or a marker for when to stop writing to the file.
- `outPosition` is `0`, suggesting that, at this breakpoint, no data has yet been written to the output file.
- `inputSignals[0]->ready()` returns `0`, indicating that, at the moment of the breakpoint, there are no data points ready to be written from the input signal.

The critical insight here is the last point: since `inputSignals[0]->ready()` returns `0`, it means the `SaveAscii::runBlock` has no data to process and write to the output file, which could explain why your output file is empty.

### Potential Issues and Solutions:

1. **Data Flow**: Ensure that `outputSignalRX`, which feeds into `SaveAscii` as `inputSignals[0]`, is correctly receiving data from `messageHandlerRX`. It's possible that `messageHandlerRX` is not outputting data as expected, or there is a synchronization issue where `SaveAscii::runBlock` is called before `messageHandlerRX` has processed and output any data.

2. **Timing/Synchronization**: Consider the order and timing of when `runBlock` methods are called for your message handlers and `SaveAscii`. You may need to ensure that `messageHandlerRX.runBlock()` has the chance to process and forward data to `SaveAscii` before `SaveAscii::runBlock()` is called.

3. **Debugging Further**:
   - **Inspect `messageHandlerRX` Output**: Use GDB to inspect the state of `outputSignalRX` after `messageHandlerRX.runBlock()` has been called but before `SaveAscii.runBlock()` is executed. This can help confirm whether data is being passed through as expected.
   - **Check `messageHandlerRX` Logic**: Ensure that `messageHandlerRX` is correctly processing its input and that its output (`outputSignalRX`) is set up correctly to store and forward the processed data.

4. **Review `SaveAscii` Initialization**: Double-check how `SaveAscii` is initialized, especially regarding `inputSignals` and `outputSignals`. Ensure that these are correctly linked to the expected source and destinations.

By addressing these points and potentially adjusting the flow or synchronization of your data processing, you should be able to pinpoint and resolve the issue with the empty output file.

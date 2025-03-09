##### Osnova:
1. importujeme includes
- stdio, unistd, string, stdlib
- sys/socket.h a netinet/in

2. nadefinujeme strukturu 
3. pripravime si taky globalni variables, ktery jsou temer nutny pro implementaci
4. pripravime si error handling funkci
5. pripravime si send to all clients funkci pro komunikaci
- ke komunikaci budeme pouzivat prave tuhle send_to_all funkci, v kombinaci se sprintf importovanou funkci, kterou pouzivame protoze musime komunikaci trochu bufferovat

6. napiseme main funkci 
- Ta ma nasledujici strukturu:
- 1. arg check
- 2. setup socketu
- 3. inicializace fd a client arraye
- 4. konfigurace serverove adresy
- 5. bind and listen na socketu serveru
- 6. server loop while(TRUE)


🟦 Monitor Inteligente de Conforto Ambiental e Bem-Estar no Trabalho
Sistema IoT com ESP32 + Sensores + MQTT + Alertas Inteligentes

Grupo: 
-Mikael de Albuquerque Santana RM 566507
-Felipe Ramalho Junqueira Berto RM 562148

🔗 Simulação no Wokwi:
https://wokwi.com/projects/447987905229798401

🎥 Vídeo Explicativo:

📌 1. Descrição do Problema

Ambientes de trabalho frequentemente apresentam condições inadequadas, como:

Temperatura elevada

Baixa umidade

Iluminação insuficiente

Falta de ventilação

Permanência prolongada sem movimento

Maus hábitos posturais

Esses fatores podem causar:

Fadiga

Problemas respiratórios

Desconforto visual

Desidratação

Queda de produtividade

Riscos ergonômicos

Apesar disso, a maioria dos ambientes não possui monitoramento automático para avisar colaboradores quando as condições se tornam prejudiciais.

📌 2. Descrição da Solução

Para resolver esse problema, foi desenvolvido um sistema IoT inteligente baseado em ESP32 que:

✔ monitora temperatura, umidade, iluminação e presença
✔ envia dados continuamente via MQTT
✔ gera alertas sonoros, visuais e digitais
✔ detecta ausência prolongada e sugere pausas
✔ possui modo inclusão (LED especial para surdos)
✔ evita repetição de alertas por meio de sistema anti-spam
✔ permite integração com dashboards e automações

O objetivo é promover saúde, ergonomia e bem-estar através de monitoramento contínuo e alertas automáticos.

📌 3. Principais Funcionalidades
🔍 Monitoramento ambiental:

Temperatura

Umidade

Iluminação (LDR)

Presença (sensor PIR)

🚨 Alertas inteligentes:

Ambiente quente ou seco

Iluminação insuficiente

Ausência prolongada

Lembretes de pausa e hidratação

Acessibilidade com LED de inclusão

📡 Comunicação MQTT:

Envia dados em tempo real

Tópicos dedicados para dados, comandos e alertas

Compatível com EMQX, Node-RED, Mosquitto, etc.

🧠 Sistema anti-spam:

Cada evento crítico dispara apenas um alerta, evitando excesso de notificações.

📌 4. Estrutura do Projeto
📁 monitor-ambiental-iot/
 ├── README.md
 ├── src/
 │     └── main.ino       ← código do ESP32
 ├── imagens/
 │     ├── wokwi.png
 │     ├── emqx.png
 │     └── arquitetura.png
 └── LICENSE

📌 5. Imagens do Projeto

(Adicione imagens suas reais nesta pasta após exportar)

🟦 Simulação no Wokwi
![Simulação no Wokwi](imagens/wokwi.png)

🟩 Leituras e alertas no EMQX
![Dashboard MQTT](imagens/emqx.png)

🧩 Arquitetura IoT
![Arquitetura do Sistema](imagens/arquitetura.png)

📌 6. Instruções de Uso
✔ Como rodar a simulação no Wokwi:

Clique no link:
https://wokwi.com/projects/447987905229798401

Aperte Run

O ESP32 começará a:

conectar ao Wi-Fi simulado

conectar ao servidor MQTT

publicar dados a cada 2s

✔ Como visualizar os dados no EMQX:

Acesse seu servidor EMQX

Vá em: Tools → WebSocket Client

Crie uma nova assinatura (New Subscription)

Insira:

/sensor/#


Agora você verá:

dados dos sensores

alertas

status de presença

📌 7. Instruções de Replicação
🔧 Materiais necessários (se for físico)

ESP32

Sensor DHT22

Sensor LDR + resistor

Sensor PIR

LED comum

LED para inclusão

Buzzer passivo

Relé 5V (opcional)

Protoboard e jumpers

📡 Configuração MQTT

Utilize o servidor passado pelo professor:

Host: 44.223.43.74
Porta: 1883


Tópicos utilizados:

/sensor/dados
/sensor/alertas
/esp32/comandos

🟦 Reproduzindo em qualquer computador

Instale o Arduino IDE

Instale a placa ESP32

Instale as bibliotecas:

DHTesp

PubSubClient

ArduinoJson

Faça upload do arquivo main.ino

Abra o monitor serial para visualizar os logs

📌 8. Link do Vídeo Explicativo

Após gravar sua apresentação, basta adicionar o link aqui:

https://youtu.be/SEU_VIDEO_AQUI

📌 9. Considerações Finais

Este projeto demonstra como tecnologias IoT podem contribuir para:

saúde ocupacional

ergonomia

acessibilidade

segurança no trabalho

análise ambiental

automação inteligente

A solução é totalmente replicável, expansível e ideal para ambientes corporativos, acadêmicos ou domésticos.

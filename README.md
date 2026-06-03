<img width="1895" height="942" alt="image" src="https://github.com/user-attachments/assets/26d8b9f0-25f7-42a1-bc4b-cac3ab10ce4a" />


# 🌡️ Módulo de Monitoramento IoT — ESP32 + Wokwi + MQTT + Dashboard Web

> Sistema de coleta, transmissão e análise de grandezas físicas em tempo real, desenvolvido como parte de trabalho acadêmico (TCC). Utiliza simulação digital de hardware via Wokwi, comunicação via protocolo MQTT e visualização em dashboard web interativo.

---

## 📋 Índice

- [Visão Geral](#visão-geral)
- [Demonstração](#demonstração)
- [Arquitetura do Sistema](#arquitetura-do-sistema)
- [Grandezas Monitoradas](#grandezas-monitoradas)
- [Tecnologias Utilizadas](#tecnologias-utilizadas)
- [Estrutura do Projeto](#estrutura-do-projeto)
- [Como Executar](#como-executar)
- [Formato dos Dados](#formato-dos-dados)
- [Funcionalidades do Dashboard](#funcionalidades-do-dashboard)
- [Protocolo MQTT](#protocolo-mqtt)

---

## Visão Geral

Este projeto implementa um módulo de aquisição de dados baseado em **ESP32**, simulado digitalmente pela plataforma **Wokwi**. Os dados coletados pelos sensores são transmitidos via protocolo **MQTT** para um broker público na nuvem (**HiveMQ**) e consumidos por um **dashboard web** desenvolvido em HTML, CSS e JavaScript puro — sem frameworks, sem servidores, sem custo.

O sistema foi projetado para ser executado inteiramente pelo navegador, tornando a demonstração simples e acessível em qualquer ambiente.

```
Sem hardware físico. Sem instalação de servidores. Funciona 100% no navegador.
```

---

## Demonstração

Para visualizar o sistema funcionando:

1. Acesse o projeto no **Wokwi** → inicie a simulação (▶ Play)
2. Abra o `dashboard.html` no Chrome ou Edge
3. Os dados dos sensores aparecerão nos gráficos em tempo real

> O dashboard também pode ser acessado via GitHub Pages após o deploy (ver seção [Como Executar](#como-executar)).

---

## Arquitetura do Sistema

```
┌─────────────────────────────────────────────────────────────┐
│                        WOKWI                                │
│                                                             │
│   ┌──────────┐   ┌───────┐   ┌──────┐   ┌───────────────┐ │
│   │  DHT22   │   │  LDR  │   │ACS712│   │ Div. Tensão   │ │
│   │Temp/Umid │   │  Lux  │   │Corr. │   │   Tensão      │ │
│   └────┬─────┘   └───┬───┘   └──┬───┘   └──────┬────────┘ │
│        └─────────────┴──────────┴───────────────┘          │
│                            │                                │
│                     ┌──────▼──────┐                         │
│                     │   ESP32     │                         │
│                     │  DevKit V1  │                         │
│                     └──────┬──────┘                         │
└────────────────────────────┼────────────────────────────────┘
                             │
                  MQTT / TCP │ porta 1883
                  JSON a cada 2 segundos
                             │
                    ┌────────▼────────┐
                    │  HiveMQ Broker  │
                    │ broker.hivemq   │
                    │     .com        │
                    └────────┬────────┘
                             │
              WebSocket TLS  │ porta 8884
                             │
                    ┌────────▼────────┐
                    │  Dashboard Web  │
                    │  dashboard.html │
                    │                 │
                    │  • Gráficos     │
                    │  • Estatísticas │
                    │  • Alertas      │
                    │  • Export CSV   │
                    └─────────────────┘
```

---

## Grandezas Monitoradas

| Grandeza | Sensor | Pino ESP32 | Faixa | Observação |
|---|---|---|---|---|
| Temperatura | DHT22 | GPIO 4 | -40°C a 80°C | Pull-up 10kΩ obrigatório |
| Umidade relativa | DHT22 | GPIO 4 | 0% a 100% | Mesmo sensor da temperatura |
| Luminosidade | LDR + R 10kΩ | GPIO 34 | 0% a 100% | Divisor resistivo com 3.3V |
| Tensão elétrica | Divisor R1=10kΩ / R2=3.3kΩ | GPIO 35 | 0V a 15.6V | Escala para ADC 3.3V |
| Corrente elétrica | ACS712 (5A) | GPIO 32 | -5A a +5A | 185mV/A, zero em 2.5V |

> **GPIO 34 e 35** são pinos *input-only* do ESP32 — não possuem pull-up interno e são ideais para leitura ADC.

---

## Tecnologias Utilizadas

### Hardware (simulado)
| Componente | Função |
|---|---|
| ESP32 DevKit V1 | Microcontrolador principal |
| DHT22 (AM2302) | Sensor de temperatura e umidade |
| LDR + resistor 10kΩ | Sensor de luminosidade |
| Resistores 10kΩ e 3.3kΩ | Divisor de tensão para medição de tensão elétrica |
| ACS712 (5A) | Sensor de corrente elétrica |

### Plataformas
| Plataforma | Uso | Link |
|---|---|---|
| Wokwi | Simulação do hardware ESP32 | [wokwi.com](https://wokwi.com) |
| HiveMQ | Broker MQTT público e gratuito | [broker.hivemq.com](https://broker.hivemq.com) |
| GitHub Pages | Hospedagem do dashboard (opcional) | [pages.github.com](https://pages.github.com) |

### Linguagens
| Linguagem | Onde é usada |
|---|---|
| C++ (Arduino) | Firmware do ESP32 (`esp32_tcc.ino`) |
| JavaScript | Lógica do dashboard (conexão MQTT, gráficos, estatísticas) |
| HTML5 / CSS3 | Interface do dashboard |
| JSON | Formato de comunicação entre ESP32 e dashboard |

### Bibliotecas — ESP32 (Arduino)
| Biblioteca | Versão | Função |
|---|---|---|
| DHT sensor library | 1.4.4 | Leitura do sensor DHT22 |
| ArduinoJson | 6.21.3 | Serialização dos dados em JSON |
| PubSubClient | 2.8.0 | Comunicação MQTT |
| WiFi | built-in | Conexão à rede Wi-Fi (Wokwi-GUEST) |

### Bibliotecas — Dashboard (JavaScript, via CDN)
| Biblioteca | Versão | Função |
|---|---|---|
| mqtt.js | 5.3.4 | Conexão WebSocket ao broker MQTT |
| Chart.js | 4.4.1 | Geração dos gráficos de série temporal |

---

## Estrutura do Projeto

```
📦 tcc-iot/
├── 📄 esp32_tcc.ino      # Firmware completo do ESP32
├── 📄 diagram.json       # Circuito Wokwi (componentes + conexões)
├── 📄 libraries.txt      # Lista de bibliotecas para o Wokwi
├── 📄 dashboard.html     # Dashboard web completo
├── 📄 resumo_projeto.md  # Resumo técnico do projeto
└── 📄 README.md          # Este arquivo
```

---

## Como Executar

### Pré-requisitos
- Navegador Chrome ou Edge (recomendado)
- Conta gratuita no [Wokwi](https://wokwi.com)
- Nenhuma instalação adicional necessária

---

### Passo 1 — Configurar o Wokwi

1. Acesse [wokwi.com](https://wokwi.com) → **New Project** → **ESP32**
2. Substitua o conteúdo do `diagram.json` pelo arquivo deste repositório
3. Cole o conteúdo do `esp32_tcc.ino` na aba de código
4. No **Library Manager** (ícone 📚), instale:
   - `DHT sensor library` — por Adafruit
   - `ArduinoJson` — por Benoit Blanchon
   - `PubSubClient` — por Nick O'Leary

> O Wi-Fi do Wokwi usa a rede virtual `Wokwi-GUEST` (sem senha). O código já está configurado para ela — não é necessário inserir sua senha de Wi-Fi real.

---

### Passo 2 — Iniciar a simulação

1. Clique em **▶ Play** no Wokwi
2. Abra o **Serial Monitor** (painel inferior)
3. Aguarde as mensagens de confirmação:

```
[WiFi] Conectado! IP: 10.10.0.2
[MQTT] Conectando ao broker broker.hivemq.com... conectado!
[MQTT #1] Publicado: {"id":1,"ts":2001,"temp":25.3,...}
```

---

### Passo 3 — Abrir o dashboard

1. Faça o download do `dashboard.html`
2. Abra-o diretamente no **Chrome ou Edge** (duplo clique no arquivo)
3. O dashboard conecta ao broker automaticamente
4. Em até 15 segundos, o indicador **ESP32** ficará verde e os gráficos começarão a atualizar

---

### Passo 4 (opcional) — Deploy no GitHub Pages

Para gerar uma URL pública e acessível de qualquer lugar:

1. Suba o `dashboard.html` para um repositório público no GitHub
2. Vá em **Settings → Pages → Branch: main → Save**
3. Acesse: `https://<seu-usuario>.github.io/<repositorio>/dashboard.html`

---

## Formato dos Dados

Cada mensagem publicada pelo ESP32 no tópico `tcc/modulo1/sensores` segue o formato JSON abaixo:

```json
{
  "id": 42,
  "ts": 84000,
  "temp": 25.3,
  "umid": 61.0,
  "tensao": 5.12,
  "corrente": 0.38,
  "lux_raw": 74.2,
  "modulo": "modulo1"
}
```

| Campo | Tipo | Descrição |
|---|---|---|
| `id` | inteiro | Contador sequencial de leituras |
| `ts` | inteiro | Timestamp em ms desde o boot do ESP32 |
| `temp` | decimal | Temperatura em °C (1 casa decimal) |
| `umid` | decimal | Umidade relativa em % (1 casa decimal) |
| `tensao` | decimal | Tensão elétrica em V (2 casas decimais) |
| `corrente` | decimal | Corrente elétrica em A (2 casas decimais) |
| `lux_raw` | decimal | Luminosidade em % (0 = escuro, 100 = máximo) |
| `modulo` | string | Identificador do módulo de coleta |

---

## Funcionalidades do Dashboard

### Visualização em tempo real
- Cards de métricas com valor atual e delta em relação à leitura anterior
- 4 gráficos de série temporal com janela deslizante de 60 leituras
- Indicadores de status: conexão com broker e status do ESP32

### Análise estatística
Para cada grandeza monitorada, o sistema calcula automaticamente:
- Valor mínimo e máximo registrados na sessão
- Média aritmética e mediana
- Desvio padrão
- Tendência linear (crescente / decrescente / estável)

### Sistema de alertas
- Thresholds configuráveis pelo usuário para cada variável
- Badge de alerta visual no card da grandeza
- Registro automático no log de eventos com timestamp
- Detecção de inatividade do ESP32 (offline após 10s sem dados)

### Exportação e persistência
- Exportação de todos os dados da sessão em **CSV** com um clique
- Histórico persistido no `localStorage` do navegador (sobrevive ao fechar o browser)
- Restauração automática do histórico ao reabrir o dashboard

### Comandos remotos
- Envio de comandos para o ESP32 via tópico MQTT `tcc/modulo1/cmd`
- Suporte ao comando `reset` para zerar o contador de leituras

---

## Protocolo MQTT

**MQTT** (Message Queuing Telemetry Transport) é o protocolo padrão da indústria para comunicação em dispositivos IoT. Opera no modelo **publish/subscribe**:

```
ESP32 (publisher)  →  publica em  →  tcc/modulo1/sensores
Dashboard (subscriber)  →  assina  →  tcc/modulo1/sensores
Dashboard (publisher)   →  publica em  →  tcc/modulo1/cmd
ESP32 (subscriber)      →  assina  →  tcc/modulo1/cmd
```

| Parâmetro | Valor |
|---|---|
| Broker | `broker.hivemq.com` |
| Porta TCP (ESP32) | `1883` |
| Porta WebSocket/TLS (dashboard) | `8884` |
| Tópico de dados | `tcc/modulo1/sensores` |
| Tópico de comandos | `tcc/modulo1/cmd` |
| QoS | 0 |
| Retain | true (último valor sempre disponível) |
| Intervalo de publicação | 2 segundos |

---

*Desenvolvido para TCC — Sistema de Monitoramento IoT com ESP32 simulado via Wokwi, protocolo MQTT e dashboard web.*

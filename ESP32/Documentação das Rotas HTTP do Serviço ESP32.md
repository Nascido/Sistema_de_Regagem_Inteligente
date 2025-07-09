<img src="https://r2cdn.perplexity.ai/pplx-full-logo-primary-dark%402x.png" class="logo" width="120"/>

# Documentação das Rotas HTTP do Serviço ESP32

Este documento detalha todas as rotas HTTP disponíveis no serviço embarcado do ESP32, descrevendo suas funcionalidades, parâmetros, exemplos de uso e respostas esperadas. O objetivo é fornecer uma referência clara para integração, testes e manutenção do sistema.

## Sumário das Rotas

| Método | Rota | Descrição | Autenticação | Parâmetros |
| :-- | :-- | :-- | :-- | :-- |
| GET | /status | Consulta o status atual do sistema | Não | Nenhum |
| GET | /set_umidade | Define o limiar de umidade | Não | valor (query) |
| GET | /restart | Reinicia o ESP32 | Não | Nenhum |
| GET | /ativar_bomba | Ativa a bomba imediatamente | Não | Nenhum |

## 1. `/status`

### Método

`GET`

### Descrição

Retorna o status atual dos sensores e do sistema, incluindo valores de umidade, LDR, estado do relé (bomba) e limiar configurado.

### Parâmetros

Nenhum.

### Exemplo de Requisição

```
GET http://<ip_do_esp32>/status
```


### Exemplo de Resposta

```json
{
  "umidade": 2030,
  "ldr": 1020,
  "rele": 0,
  "umidade_limiar": 2000
}
```

- **umidade**: Valor lido no sensor de umidade (ADC).
- **ldr**: Valor lido no sensor de luminosidade (ADC).
- **rele**: Estado do relé (0 = desligado, 1 = ligado).
- **umidade_limiar**: Limiar atual para ativação automática da bomba.


## 2. `/set_umidade`

### Método

`GET`

### Descrição

Define um novo limiar de umidade para ativação automática da bomba.

### Parâmetros

| Nome | Tipo | Obrigatório | Descrição |
| :-- | :-- | :-- | :-- |
| valor | int | Sim | Novo limiar (1 a 4095, ADC 12 bits) |

### Exemplo de Requisição

```
GET http://<ip_do_esp32>/set_umidade?valor=2500
```


### Respostas Possíveis

- **200 OK**: Limiar atualizado com sucesso.
    - Corpo: `"Novo limiar de umidade definido para: 2500"`
- **422 Unprocessable Entity**: Valor inválido.
    - Corpo: `"Valor inválido para limiar. Use um inteiro entre 1 e 4095."`
- **400 Bad Request**: Parâmetro ausente.
    - Corpo: `"Parametro 'valor' ausente."`


## 3. `/restart`

### Método

`GET`

### Descrição

Reinicia o ESP32 remotamente.

### Parâmetros

Nenhum.

### Exemplo de Requisição

```
GET http://<ip_do_esp32>/restart
```


### Respostas Possíveis

- **200 OK**: Confirmação do comando de reinício.
    - Corpo: `"Reiniciando ESP32..."`


## 4. `/ativar_bomba`

### Método

`GET`

### Descrição

Ativa a bomba imediatamente por 2 segundos, independentemente dos valores dos sensores.

### Parâmetros

Nenhum.

### Exemplo de Requisição

```
GET http://<ip_do_esp32>/ativar_bomba
```


### Respostas Possíveis

- **200 OK**: Bomba será ativada.
    - Corpo: `"Bomba será ativada imediatamente por 2 segundos."`
- **409 Conflict**: Bomba já está ligada ou aguardando ciclo.
    - Corpo: `"Bomba já está ligada ou aguardando ciclo."`


## Observações Gerais

- **Todas as rotas utilizam o método GET** para facilitar testes via navegador ou ferramentas como Postman/cURL.
- **Autenticação**: Atualmente, não há proteção por senha/token. Recomenda-se implementar autenticação para rotas sensíveis como `/restart` e `/ativar_bomba`.
- **Respostas em JSON** são utilizadas apenas na rota `/status`; as demais retornam texto simples.
- **Endereço IP**: Substitua `<ip_do_esp32>` pelo IP atribuído ao ESP32 na sua rede WiFi, mostrado no display LCD após inicialização.


## Exemplos de Integração

### cURL

```sh
# Consultar status
curl http://192.168.0.50/status

# Definir limiar de umidade
curl "http://192.168.0.50/set_umidade?valor=2200"

# Ativar bomba imediatamente
curl http://192.168.0.50/ativar_bomba

# Reiniciar ESP32
curl http://192.168.0.50/restart
```


## Próximos Passos Recomendados

- Implementar autenticação nas rotas críticas.
- Adicionar logs de eventos para auditoria.
- Permitir configuração do tempo de ativação da bomba via parâmetro.
- Documentar possíveis códigos de erro HTTP e mensagens de resposta.

Esta documentação pode ser incorporada ao README do projeto ou utilizada como referência para integração com sistemas externos e aplicações móveis/web.


package services

import (
    "bytes"
    "encoding/json"
    "fmt"
    "io/ioutil"
    "net/http"
    "os"
)

var esp32BaseURL = os.Getenv("ESP32_URL") // Ex: http://192.168.1.100

type ESPStatus struct {
    Umidade       int `json:"umidade"`
    LDR           int `json:"ldr"`
    Rele          int `json:"rele"`
    UmidadeLimiar int `json:"umidade_limiar"`
}

func GetESPStatus() (*ESPStatus, error) {
    resp, err := http.Get(fmt.Sprintf("%s/status", esp32BaseURL))
    if err != nil {
        return nil, err
    }
    defer resp.Body.Close()
    var status ESPStatus
    if err := json.NewDecoder(resp.Body).Decode(&status); err != nil {
        return nil, err
    }
    return &status, nil
}

func SetESPUmidade(valor int) error {
    url := fmt.Sprintf("%s/set_umidade?valor=%d", esp32BaseURL, valor)
    resp, err := http.Get(url)
    if err != nil {
        return err
    }
    defer resp.Body.Close()
    if resp.StatusCode != 200 {
        body, _ := ioutil.ReadAll(resp.Body)
        return fmt.Errorf(string(body))
    }
    return nil
}

func RestartESP() error {
    url := fmt.Sprintf("%s/restart", esp32BaseURL)
    resp, err := http.Get(url)
    if err != nil {
        return err
    }
    defer resp.Body.Close()
    if resp.StatusCode != 200 {
        body, _ := ioutil.ReadAll(resp.Body)
        return fmt.Errorf(string(body))
    }
    return nil
}

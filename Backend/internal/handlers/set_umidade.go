package handlers

import (
    "net/http"
    "strconv"
    "meuprojeto/internal/services"
)

func SetUmidade(w http.ResponseWriter, r *http.Request) {
    valorStr := r.URL.Query().Get("valor")
    if valorStr == "" {
        http.Error(w, "Parametro 'valor' ausente.", http.StatusBadRequest)
        return
    }
    valor, err := strconv.Atoi(valorStr)
    if err != nil || valor < 1 || valor > 4095 {
        http.Error(w, "Valor inválido para limiar. Use um inteiro entre 1 e 4095.", http.StatusUnprocessableEntity)
        return
    }
    err = services.SetESPUmidade(valor)
    if err != nil {
        http.Error(w, "Falha ao definir limiar no ESP32", http.StatusInternalServerError)
        return
    }
    w.Write([]byte("Novo limiar de umidade definido para: " + valorStr))
}

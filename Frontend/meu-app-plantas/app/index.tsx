import React, { useState } from "react";
import { StyleSheet, View } from "react-native";
import { DADOS_DEMO } from "./api/mock_data";

import { Button, Card, Icon, Text } from "react-native-paper";
import { SafeAreaView } from "react-native-safe-area-context";

// --- Interface para os dados que vamos usar ---
interface PlantData {
  id: number;
  common_name: string;
  scientific_name: string;
  watering: string; // Ex: "Average", "Frequent", "Minimum"
  sunlight: string[]; // Ex: ["full_sun", "part_shade"]
  default_image?: {
    thumbnail: string;
  };
}

interface SensorData {
  moisture: number;
  light: number;
}

export default function MVPScreen() {
  const [sensorData, setSensorData] = useState<SensorData>({
    moisture: Math.floor(Math.random() * 101),
    light: Math.floor(Math.random() * 101),
  });

  // Define os dados da planta diretamente
  const plantData: PlantData = DADOS_DEMO as unknown as PlantData;

  const refreshData = () => {
    setSensorData({
      moisture: Math.floor(Math.random() * 101),
      light: Math.floor(Math.random() * 101),
    });
  };

  // --- Componente de Renderização Principal ---

  const renderStatusCard = () => {
    // Lógica simples de diagnóstico
    const getWateringStatus = () => {
      // Simplificando: "Average" = umidade ideal entre 40 e 70
      if (plantData.watering === "Average") {
        if (sensorData.moisture < 40)
          return {
            text: "Precisa de Água!",
            icon: "water-alert-outline",
            color: "#FF6B6B",
          };
        if (sensorData.moisture > 70)
          return {
            text: "Muita Água!",
            icon: "water-check-outline",
            color: "#4ECDC4",
          };
      }
      return { text: "Rega OK", icon: "water-check", color: "#45B7D1" };
    };

    const getLightStatus = () => {
      // Simplificando: "full_sun" = luz ideal acima de 60
      if (plantData.sunlight.includes("full_sun")) {
        if (sensorData.light < 60)
          return {
            text: "Precisa de mais Sol!",
            icon: "weather-sunny-alert",
            color: "#FFA726",
          };
      }
      return { text: "Luz Ideal", icon: "weather-sunny", color: "#FFD54F" };
    };

    const wateringStatus = getWateringStatus();
    const lightStatus = getLightStatus();

    return (
      <View style={styles.statusContainer}>
        <Card style={styles.card}>
          <Card.Cover
            source={{
              uri:
                plantData.default_image?.thumbnail ||
                "https://images.unsplash.com/photo-1416879595882-3373a0480b5b?ixlib=rb-4.0.3&auto=format&fit=crop&w=1000&q=80",
            }}
            style={{
              borderTopLeftRadius: 16,
              borderTopRightRadius: 16,
              height: 300,
            }}
            resizeMode="cover"
          />
          <Card.Title
            title={plantData.common_name}
            subtitle={plantData.scientific_name}
            titleStyle={{ fontWeight: "bold", color: "#FFFFFF", fontSize: 20 }}
            subtitleStyle={{ color: "#B0B0B0", fontSize: 14 }}
          />
          <Card.Content style={{ paddingTop: 16 }}>
            {/* Status da Umidade */}
            <View
              style={[
                styles.statusItem,
                { backgroundColor: wateringStatus.color },
              ]}
            >
              <Icon source={wateringStatus.icon} size={32} color="#1A1A1A" />
              <View style={{ flex: 1, marginLeft: 16 }}>
                <Text variant="titleMedium" style={styles.statusText}>
                  {wateringStatus.text}
                </Text>
                <Text
                  style={[styles.statusText, { fontSize: 14, opacity: 0.8 }]}
                >
                  Umidade: {sensorData.moisture}%
                </Text>
              </View>
            </View>

            {/* Status da Luz */}
            <View
              style={[
                styles.statusItem,
                { backgroundColor: lightStatus.color },
              ]}
            >
              <Icon source={lightStatus.icon} size={32} color="#1A1A1A" />
              <View style={{ flex: 1, marginLeft: 16 }}>
                <Text variant="titleMedium" style={styles.statusText}>
                  {lightStatus.text}
                </Text>
                <Text
                  style={[styles.statusText, { fontSize: 14, opacity: 0.8 }]}
                >
                  Luminosidade: {sensorData.light}%
                </Text>
              </View>
            </View>
          </Card.Content>
        </Card>
        <Button
          icon="refresh"
          mode="contained"
          onPress={refreshData}
          style={[styles.button, { backgroundColor: "#6C757D" }]}
          labelStyle={{ color: "#FFFFFF", fontSize: 16 }}
        >
          Atualizar Sensores
        </Button>
      </View>
    );
  };

  return (
    <SafeAreaView style={styles.safeArea}>
      <View style={styles.container}>
        <View style={styles.titleContainer}>
          <Text variant="headlineLarge" style={styles.title}>
            Monitor de Plantas
          </Text>
        </View>

        {renderStatusCard()}
      </View>
    </SafeAreaView>
  );
}

const styles = StyleSheet.create({
  safeArea: {
    flex: 1,
    backgroundColor: "#121212", // Fundo escuro para melhor contraste
  },
  container: {
    flex: 1,
    justifyContent: "center",
    padding: 20,
    backgroundColor: "#121212",
  },
  titleContainer: {
    width: "100%",
    alignItems: "center",
    marginBottom: 24,
  },
  statusContainer: {
    width: "100%",
    alignItems: "center",
  },
  title: {
    marginBottom: 24,
    fontWeight: "bold",
    color: "#FFFFFF", // Texto branco para contraste
    textAlign: "center",
    fontSize: 28,
  },
  button: {
    width: "100%",
    paddingVertical: 12,
    marginTop: 16,
    backgroundColor: "#4CAF50", // Verde vibrante para o botão
    borderRadius: 8,
  },
  card: {
    width: "100%",
    marginBottom: 24,
    backgroundColor: "#1E1E1E", // Fundo escuro do card
    borderRadius: 16,
    elevation: 6,
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 3 },
    shadowOpacity: 0.27,
    shadowRadius: 4.65,
  },
  statusItem: {
    flexDirection: "row",
    alignItems: "center",
    padding: 20,
    borderRadius: 12,
    marginBottom: 16,
    elevation: 2,
    shadowColor: "#000",
    shadowOffset: { width: 0, height: 1 },
    shadowOpacity: 0.22,
    shadowRadius: 2.22,
  },
  statusText: {
    marginLeft: 16,
    fontWeight: "bold",
    fontSize: 16,
    color: "#1A1A1A", // Texto escuro nos status items para contraste com o fundo colorido
  },
});

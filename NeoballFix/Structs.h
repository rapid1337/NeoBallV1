#pragma once

struct Color {
    float r, g, b, a;
    
    // Hilfsfunktion für dunklere Farbe (Schatten)
    Color darken(float factor) const {
        return { r * factor, g * factor, b * factor, a };
    }
};

enum PowerUpType { PU_WIDE, PU_FIRE, PU_MULTI };

// Struktur für Punkte-Items (Münzen/Kreise)
struct PointItem {
    float x, y;
    float velY;
    float radius;
    int value;
    Color color;
    bool active;
};
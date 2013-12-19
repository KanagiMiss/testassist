import QtQuick 1.0

Rectangle {
    width: 100
    height: 10

    gradient: Gradient {
        GradientStop {
            position: 0
            color: "#959795"
        }

        GradientStop {
            position: 0.12
            color: "#6a6d6a"
        }

        GradientStop {
            position: 0.92
            color: "#555555"
        }
    }
}

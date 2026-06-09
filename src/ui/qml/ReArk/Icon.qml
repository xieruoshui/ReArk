import QtQuick

Canvas {
    id: root

    property string name: ""
    property color color: "#1f3354"
    property real strokeWidth: 1.8
    property bool filled: false

    implicitWidth: 16
    implicitHeight: 16
    antialiasing: true

    onNameChanged: requestPaint()
    onColorChanged: requestPaint()
    onStrokeWidthChanged: requestPaint()
    onFilledChanged: requestPaint()
    onWidthChanged: requestPaint()
    onHeightChanged: requestPaint()

    onPaint: {
        const ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)

        const side = Math.min(width, height)
        const scale = side / 24
        const offsetX = (width - side) / 2
        const offsetY = (height - side) / 2

        ctx.save()
        ctx.translate(offsetX, offsetY)
        ctx.scale(scale, scale)
        ctx.lineWidth = root.strokeWidth
        ctx.strokeStyle = root.color
        ctx.fillStyle = root.color
        ctx.lineCap = "round"
        ctx.lineJoin = "round"

        if (root.name === "paperclip") {
            drawPaperclip(ctx)
        } else if (root.name === "diamond") {
            drawDiamond(ctx)
        } else if (root.name === "arrow-up") {
            drawArrowUp(ctx)
        } else if (root.name === "new-chat") {
            drawNewChat(ctx)
        } else if (root.name === "sparkle") {
            drawSparkle(ctx)
        }

        ctx.restore()
    }

    function drawPaperclip(ctx) {
        ctx.beginPath()
        ctx.moveTo(9.2, 12.8)
        ctx.lineTo(14.8, 7.2)
        ctx.bezierCurveTo(16.2, 5.8, 18.4, 5.8, 19.8, 7.2)
        ctx.bezierCurveTo(21.2, 8.6, 21.2, 10.8, 19.8, 12.2)
        ctx.lineTo(11.3, 20.7)
        ctx.bezierCurveTo(8.6, 23.4, 4.2, 23.4, 1.5, 20.7)
        ctx.bezierCurveTo(-1.2, 18.0, -1.2, 13.6, 1.5, 10.9)
        ctx.lineTo(11.8, 0.6)
        ctx.stroke()
    }

    function drawDiamond(ctx) {
        ctx.beginPath()
        ctx.moveTo(12, 4)
        ctx.lineTo(20, 12)
        ctx.lineTo(12, 20)
        ctx.lineTo(4, 12)
        ctx.closePath()
        if (root.filled) {
            ctx.fill()
        } else {
            ctx.stroke()
        }
    }

    function drawArrowUp(ctx) {
        ctx.beginPath()
        ctx.moveTo(12, 19)
        ctx.lineTo(12, 6)
        ctx.moveTo(6.5, 11.5)
        ctx.lineTo(12, 6)
        ctx.lineTo(17.5, 11.5)
        ctx.stroke()
    }

    function drawNewChat(ctx) {
        ctx.beginPath()
        ctx.moveTo(6.5, 5.5)
        ctx.lineTo(17.5, 5.5)
        ctx.quadraticCurveTo(19.5, 5.5, 19.5, 7.5)
        ctx.lineTo(19.5, 15)
        ctx.quadraticCurveTo(19.5, 17, 17.5, 17)
        ctx.lineTo(11.5, 17)
        ctx.lineTo(7.8, 20)
        ctx.lineTo(8.4, 17)
        ctx.lineTo(6.5, 17)
        ctx.quadraticCurveTo(4.5, 17, 4.5, 15)
        ctx.lineTo(4.5, 7.5)
        ctx.quadraticCurveTo(4.5, 5.5, 6.5, 5.5)
        ctx.stroke()

        ctx.beginPath()
        ctx.moveTo(12, 8.8)
        ctx.lineTo(12, 13.8)
        ctx.moveTo(9.5, 11.3)
        ctx.lineTo(14.5, 11.3)
        ctx.stroke()
    }

    function drawSparkle(ctx) {
        ctx.beginPath()
        ctx.moveTo(12, 3.5)
        ctx.lineTo(13.9, 10.1)
        ctx.lineTo(20.5, 12)
        ctx.lineTo(13.9, 13.9)
        ctx.lineTo(12, 20.5)
        ctx.lineTo(10.1, 13.9)
        ctx.lineTo(3.5, 12)
        ctx.lineTo(10.1, 10.1)
        ctx.closePath()
        ctx.fill()
    }
}

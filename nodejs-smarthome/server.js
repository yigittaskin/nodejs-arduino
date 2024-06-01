const express = require('express');
const app = express();
const server = require('http').createServer(app);
const io = require('socket.io')(server);
const path = require('path');
const {
    SerialPort
} = require('serialport');
const {
    ReadlineParser
} = require('@serialport/parser-readline');

// Sunucu başlangıcı
const port = 3000;
server.listen(port, () => {
    console.log(`Server listening on port ${port}`);
});

// Public ve node_modules klasörlerine erişim için
app.use('/public', express.static(path.join(__dirname, 'public')));
app.use('/node_modules', express.static(path.join(__dirname, 'node_modules')));

// Ana sayfa yönlendirmesi
app.get('/', (req, res) => {
    res.sendFile(__dirname + "/index.html");
});


// Serial port ayarları
const portName = 'COM3'; // Arduino'nun bağlı olduğu portu burada belirtin
const serialPort = new SerialPort({
    path: portName,
    baudRate: 9600
});

serialPort.on('open', () => {
    console.log(`Serial Port ${portName} opened`);
});

serialPort.on('error', (err) => {
    console.error('Serial Port Error: ', err.message);
});

const parser = serialPort.pipe(new ReadlineParser({
    delimiter: '\n'
}));

parser.on('data', (data) => {
    const status = data.trim();
    console.log(`Arduino'dan gelen veri: ${status}`);
    io.emit('parkStatus', status);
    console.log()
});

app.get('/led1/on', (req, res) => {
    serialPort.write('1');
    res.send('LED1 açıldı');
  });
  
  app.get('/led1/off', (req, res) => {
    serialPort.write('2');
    res.send('LED1 kapatıldı');
  });
  
  app.get('/led2/on', (req, res) => {
    serialPort.write('3');
    res.send('LED2 açıldı');
  });
  
  app.get('/led2/off', (req, res) => {
    serialPort.write('4');
    res.send('LED2 kapatıldı');
  });

  app.get('/kapi/on', (req, res) => {
    serialPort.write('5');
    res.send('Kapı açıldı');
  });

  app.get('/kapi/off', (req, res) => {
    serialPort.write('6');
    res.send('Kapı kapatıldı');
  });

// Socket.IO bağlantıları
let kullanicilar = 0;

io.on('connection', (socket) => {
    console.log("Bir kullanıcı bağlandı.");
    kullanicilar++;
    console.log(kullanicilar);

    // Bağlı kullanıcı sayısını istemciye gönder
    io.emit('connectedUsers', kullanicilar);


    socket.on('disconnect', () => {
        kullanicilar--;
        console.log(kullanicilar);

        // Bağlı kullanıcı sayısını istemciye gönder
        io.emit('connectedUsers', kullanicilar);
    });
});
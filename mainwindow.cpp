#include "mainwindow.h"
#include "ui_mainwindow.h"

/////////////////////////////////////////////////////////////////////////////
/// Variaveis globais
///
QString svidref = "svpub1";
int svReadtype = 1;
float svMultiplyT = 0.01;
float svMultiplyC = 0.01;

int tdiv = 500;
float scale[7] = {1,1,1,1,1,1,1};
/////////////////////////////////////////////////////////////////////////////
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow), plotTimer(new QTimer(this))
{
    ui->setupUi(this);

    graficSetting();

    restoreData();

    // Conecte o timer para atualizar o gráfico
    plotTimer = new QTimer(this);

    connect(plotTimer, &QTimer::timeout, this, &MainWindow::updatePlot);

    data.resize(cN);
    for (int i = 0; i < cN; ++i) {
        data[i].reserve(5000000);  // Reserva espaço dentro de cada vetor interno
    }
    dataTime.reserve(5000000);

    ui->dial->setValue(50);
}
MainWindow::~MainWindow()
{
    if (clientThread) {
        disconnect(clientThread, nullptr, nullptr, nullptr);
        clientThread->running = false;
        clientThread->quit();
        clientThread->wait();
        delete clientThread;
        clientThread = nullptr;
    }

    // Salvando o estado atual da janela
    QSettings settings("myapp.ini", QSettings::IniFormat);
    settings.beginGroup("MainWindow");
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("Interface", eht);
    settings.setValue("svId", svidref);
    settings.setValue("svReadtype", svReadtype);
    settings.setValue("svMultiplyT", svMultiplyT);
    settings.setValue("svMultiplyC", svMultiplyC);
    settings.endGroup();

    delete ui;
    //   delete serverThread;
}
void MainWindow::restoreData()
{
    // Configurações do arquivo myapp.ini
    QCoreApplication::setOrganizationName("AWI");
    QCoreApplication::setApplicationName("Pnrf ATP");

    // Restaurando o estado anterior da janela
    QSettings settings("myapp.ini", QSettings::IniFormat);
    settings.beginGroup("MainWindow");

    const QByteArray geometry = settings.value("geometry", QByteArray()).toByteArray();
    if (!geometry.isEmpty()) {
        restoreGeometry(geometry);
    }
    const QByteArray state = settings.value("windowState", QByteArray()).toByteArray();
    if (!state.isEmpty()) {
        restoreState(state);
    }
    if (settings.contains("svId")){
        svidref = settings.value("svId", QString()).toString();
    }
    if (settings.contains("svReadtype")){
        svReadtype = settings.value("svReadtype", QByteArray()).toInt();
    }
    if (settings.contains("svMultiplyT")){
        svMultiplyT = settings.value("svMultiplyT", QByteArray()).toFloat();
    }
    if (settings.contains("svMultiplyC")){
        svMultiplyC = settings.value("svMultiplyC", QByteArray()).toFloat();
    }
    if (settings.contains("Interface")){
        eht = settings.value("Interface", QString()).toString();
    }else{
        eht = "3";
    }
    settings.endGroup();
}
void MainWindow::graficSetting()
{
    // Configuração inicial do QCustomPlot
    // Certifique-se de ter o QCustomPlot adicionado ao seu projeto
    QPen customPen1(QColor("blue")); // blue line
    customPen1.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    QPen customPen2(QColor("red")); // red line
    customPen2.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    QPen customPen3(QColor("green")); // green line
    customPen3.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    QPen customPen4(QColor("lightblue")); // red line
    customPen4.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    QPen customPen5(QColor("lightpink")); // green line
    customPen5.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    QPen customPen6(QColor("lightgreen")); // green line
    customPen6.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)
    QPen customPen7(QColor("Yellow")); // green line
    customPen6.setWidth(2); // Define a largura da linha como 2 pixels (ou o valor desejado)

    ui->customPlot->addGraph(); // Gráfico 1
    ui->customPlot->addGraph(); // Gráfico 2
    ui->customPlot->addGraph(); // Gráfico 3
    ui->customPlot->addGraph(); // Gráfico 4
    ui->customPlot->addGraph(); // Gráfico 5
    ui->customPlot->addGraph(); // Gráfico 6
    ui->customPlot->addGraph(); // Gráfico 7
    // Configurar as propriedades dos gráficos
    ui->customPlot->graph(0)->setPen(customPen1); // Configurar as propriedades do Gráfico 1
    ui->customPlot->graph(1)->setPen(customPen2); // Configurar as propriedades do Gráfico 2
    ui->customPlot->graph(2)->setPen(customPen3); // Configurar as propriedades do Gráfico 3
    ui->customPlot->graph(3)->setPen(customPen4); // Configurar as propriedades do Gráfico 1
    ui->customPlot->graph(4)->setPen(customPen5); // Configurar as propriedades do Gráfico 2
    ui->customPlot->graph(5)->setPen(customPen6); // Configurar as propriedades do Gráfico 3
    ui->customPlot->graph(6)->setPen(customPen7); // Configurar as propriedades do Gráfico 3
    ui->customPlot->yAxis->setRange(-5, 5);  // Ajuste conforme necessário
    // Adicionando linhas de grade pontilhadas manualmente
    for (int i = -5; i <= 5; ++i) {
        QCPItemStraightLine *gridLine = new QCPItemStraightLine(ui->customPlot);
        gridLine->setPen(QPen(Qt::gray)); // Configura a cor das linhas de grade
        gridLine->point1->setCoords(-5, i);
        gridLine->point2->setCoords(5, i);
    }

    ui->customPlot->setBackground(QBrush(QColor(25, 25, 25)));
    ui->customPlot->xAxis->setBasePen(QPen(Qt::white)); // Define a cor da linha de base do eixo x
    ui->customPlot->yAxis->setBasePen(QPen(Qt::white)); // Define a cor da linha de base do eixo y
    ui->customPlot->xAxis->setTickPen(QPen(Qt::white)); // Define a cor dos ticks do eixo x
    ui->customPlot->yAxis->setTickPen(QPen(Qt::white)); // Define a cor dos ticks do eixo y
    ui->customPlot->xAxis->setTickLabelColor(Qt::white); // Define a cor dos rótulos do eixo x
    ui->customPlot->yAxis->setTickLabelColor(Qt::white);

    ui->customPlot->graph(3)->setVisible(0);
    ui->customPlot->graph(4)->setVisible(0);
    ui->customPlot->graph(5)->setVisible(0);
    ui->customPlot->graph(6)->setVisible(0);
}

/////////////////////////////////////////////////////////////////////////////
/// \brief Thread Handle
///
void MainWindow::onThreadDestroyed(QObject *obj){
    if (obj == clientThread) {
        ui->actionStart->setText("Start");
    }
}
/////////////////////////////////////////////////////////////////////////////
/// \brief Button and stuffs
///
void MainWindow::on_actionStart_triggered()
{
    if(ui->actionStart->text() == "Start"){
        clientThread = new ClientThread(this);
        // Conecte o sinal updateUI à slot onUpdateUI
        connect(clientThread, &ClientThread::finished, clientThread, &ClientThread::deleteLater);
        connect(clientThread, &ClientThread::destroyed, this, &MainWindow::onThreadDestroyed);
        connect(clientThread, &ClientThread::updateUI, this, &MainWindow::onUpdateUI);
        connect(clientThread, &ClientThread::updateData, this, &MainWindow::onUpdateData);
        connect(clientThread, &ClientThread::updateSvIdList, this, &MainWindow::updateSvList);

        clientThread->eht = eht;
        clientThread->start();
        // Inicie o timer para atualizar o gráfico a cada intervalo
        plotTimer->start(83);  // Ajuste conforme necessário
        ui->actionStart->setText("Stop");
    }else{
        plotTimer->stop();
        clientThread->requestInterruption();
    }
}
void MainWindow::onUpdateUI(const QString msg){
    ui->statusbar->showMessage(msg, 5000);
    msgapp.append(msg);
    msgapp.append("\n");
}
void MainWindow::updateSvList(const QStringList list){
    listadeSV = list;
    listaDeData.append(criarData());

    // Limpa as listas existentes
    listaExtendida.clear();
    listaDePonteiros.clear();

    // Itera sobre os elementos de listadeSV e listaDeData simultaneamente
    for (int i = 0; i < listadeSV.size() && i < listaDeData.size(); ++i) {
        QString nomeItem = listadeSV.at(i); // Obtém o nome do item
        RequestType& vetorData = listaDeData[i]; // Obtém o vetor correspondente em listaDeData

        // Cria os elementos estendidos para cada nome de item
        for (int j = 0; j < 8; ++j) {
            QString nomeEstendido = nomeItem + " - " + IndexStr.at(j);
            listaExtendida.append(nomeEstendido);

            // Cria um ponteiro para o vetor correspondente em listaDeData
            listaDePonteiros.append(&vetorData[j]);
        }
    }

    ui->comboBox->clear();
    ui->comboBox->addItems(listaExtendida);
    ui->comboBox_2->clear();
    ui->comboBox_2->addItems(listaExtendida);
    ui->comboBox_3->clear();
    ui->comboBox_3->addItems(listaExtendida);
    ui->comboBox_4->clear();
    ui->comboBox_4->addItems(listaExtendida);
    ui->comboBox_5->clear();
    ui->comboBox_5->addItems(listaExtendida);
    ui->comboBox_6->clear();
    ui->comboBox_6->addItems(listaExtendida);
    ui->comboBox_7->clear();
    ui->comboBox_7->addItems(listaExtendida);
    ui->comboBox_8->clear();
    ui->comboBox_8->addItems(listaExtendida);
}
void MainWindow::on_actionShow_Messages_triggered()
{
    QMessageBox::information(this, tr("Título da mensagem"), msgapp);
}
void MainWindow::onUpdateData(const QVector<float> &vetor, const Timestamp &tempo, int index)
{
    pv[index]++;
    if (pv[index] >= 16777216) pv[index]=0;

    listaDeData[index][0][pv[index]] = vetor[0];
    listaDeData[index][1][pv[index]] = vetor[1];
    listaDeData[index][2][pv[index]] = vetor[2];
    listaDeData[index][3][pv[index]] = vetor[3];
    listaDeData[index][4][pv[index]] = vetor[4];
    listaDeData[index][5][pv[index]] = vetor[5];
    listaDeData[index][6][pv[index]] = vetor[6];
    listaDeData[index][7][pv[index]] = vetor[7];

    quint64 t = Timestamp_getTimeInNs(const_cast<Timestamp*>(&tempo));
    dataTime.append(t);

}
void MainWindow::updatePlot()
{
    qint64 pos = 0;
    static qint64 last_pv = 0;

    if(ptrTr != nullptr)
    {
        pos = pv[ptrindTr];
        if (pos <= last_pv){
            last_pv = pos;
            return;
        }
        int size = pos - last_pv;
        qint64 i1 = pv[ptrId[1]>>3] - size;
        qint64 i2 = pv[ptrId[2]>>3] - size;
        qint64 i3 = pv[ptrId[3]>>3] - size;
        qint64 i4 = pv[ptrId[4]>>3] - size;
        qint64 i5 = pv[ptrId[5]>>3] - size;
        qint64 i6 = pv[ptrId[6]>>3] - size;
        qint64 i7 = pv[ptrId[7]>>3] - size;

        bool noTg = true;
        int tg_pos;
        for(int i=0; i<size; i++)
        {
            if(ptrG1 != nullptr) ui->customPlot->graph(0)->addData(i+last_pv, ptrG1->at(i+i1)*scale[0]);
            if(ptrG2 != nullptr) ui->customPlot->graph(1)->addData(i+last_pv, ptrG2->at(i+i2)*scale[1]);
            if(ptrG3 != nullptr) ui->customPlot->graph(2)->addData(i+last_pv, ptrG3->at(i+i3)*scale[2]);
            if(ptrG4 != nullptr) ui->customPlot->graph(3)->addData(i+last_pv, ptrG4->at(i+i4)*scale[3]);
            if(ptrG5 != nullptr) ui->customPlot->graph(4)->addData(i+last_pv, ptrG5->at(i+i5)*scale[4]);
            if(ptrG6 != nullptr) ui->customPlot->graph(5)->addData(i+last_pv, ptrG6->at(i+i6)*scale[5]);
            if(ptrG7 != nullptr) ui->customPlot->graph(6)->addData(i+last_pv, ptrG7->at(i+i7)*scale[6]);

            if(ptrTr->at(i+last_pv) >= trigger && ptrTr->at(i+last_pv-1) < trigger && noTg){
                noTg = false;
                tg_pos = i+last_pv;
            }
        }
        if(noTg){
            return;
        }
        // Rescale e replot
        ui->customPlot->xAxis->setRange(tg_pos, tdiv, Qt::AlignCenter);

        ui->customPlot->replot(); //

        last_pv = pos;
    }

}
/////////////////////////////////////////////////////////////////////////////
/// \brief Othres
///
void MainWindow::on_dial_valueChanged(int value)
{
    tdiv = (value*10);
}
void MainWindow::on_comboBox_2_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrTr = listaDePonteiros.at(index);
        ptrindTr = index >> 3;
    } else {
        ptrTr = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}

float MainWindow::onDialChange(int value, QString type){

    int m = ((value-1) / 3) - 3;
    int r = value % 3;

    double s = qPow(10, m);
    double d = 1;

    if(r == 0) d = 5*s;
    if(r == 1) d = 1*s;
    if(r == 2) d = 2*s;

    if (m < 0) {
        // Escala em mV
        scaleText = QString("%1 m%2").arg((int)(d*1000)).arg(type);
    } else if (m >= 0 && m < 3) {
        // Escala em V
        scaleText = QString("%1 %2").arg((int)d).arg(type);
    } else if (m >= 3 && m < 6) {
        // Escala em kV
        scaleText = QString("%1 k%2").arg((int)(d/1000)).arg(type);
    } else if (m >= 6 && m <= 9) {
        // Escala em MV
        scaleText = QString("%1 M%2").arg((int)(d/1000000)).arg(type);
    }

    return 1.0/d;
}

void MainWindow::on_dial_2_valueChanged(int value)
{
    QString str;
    if (ptrG1 != nullptr){
        str = listaExtendida.at(ptrId[1]);
        QChar penultimo = str.at(str.length() - 2);
        if (penultimo == 'I') penultimo = 'A';
        str = penultimo;
    }
    scale[0] = onDialChange(value, str + "/div");

    ui->label_2->setText(scaleText);
}
void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrG1 = listaDePonteiros.at(index);
        ptrId[1] = index;
    } else {
        ptrG1 = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}


void MainWindow::on_dial_3_valueChanged(int value)
{
    QString str;
    if (ptrG2 != nullptr){
        str = listaExtendida.at(ptrId[2]);
        QChar penultimo = str.at(str.length() - 2);
        if (penultimo == 'I') penultimo = 'A';
        str = penultimo;
    }
    scale[1] = onDialChange(value, str + "/div");

    ui->label_4->setText(scaleText);
}
void MainWindow::on_comboBox_3_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrG2 = listaDePonteiros.at(index);
        ptrId[2] = index;
    } else {
        ptrG2 = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}

void MainWindow::on_dial_4_valueChanged(int value)
{
    QString str;
    if (ptrG3 != nullptr){
        str = listaExtendida.at(ptrId[3]);
        QChar penultimo = str.at(str.length() - 2);
        if (penultimo == 'I') penultimo = 'A';
        str = penultimo;
    }
    scale[2] = onDialChange(value, str + "/div");

    ui->label_3->setText(scaleText);
}
void MainWindow::on_comboBox_4_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrG3 = listaDePonteiros.at(index);
        ptrId[3] = index;
    } else {
        ptrG3 = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}
void MainWindow::on_dial_5_valueChanged(int value)
{
    QString str;
    if (ptrG4 != nullptr){
        str = listaExtendida.at(ptrId[4]);
        QChar penultimo = str.at(str.length() - 2);
        if (penultimo == 'I') penultimo = 'A';
        str = penultimo;
    }
    scale[3] = onDialChange(value, str + "/div");

    ui->label_8->setText(scaleText);
}
void MainWindow::on_comboBox_5_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrG4 = listaDePonteiros.at(index);
        ptrId[4] = index;
    } else {
        ptrG4 = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}
void MainWindow::on_dial_6_valueChanged(int value)
{
    QString str;
    if (ptrG5 != nullptr){
        str = listaExtendida.at(ptrId[5]);
        QChar penultimo = str.at(str.length() - 2);
        if (penultimo == 'I') penultimo = 'A';
        str = penultimo;
    }
    scale[4] = onDialChange(value, str + "/div");

    ui->label_5->setText(scaleText);
}
void MainWindow::on_comboBox_6_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrG5 = listaDePonteiros.at(index);
        ptrId[5] = index;
    } else {
        ptrG5 = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}
void MainWindow::on_dial_7_valueChanged(int value)
{
    QString str;
    if (ptrG6 != nullptr){
        str = listaExtendida.at(ptrId[6]);
        QChar penultimo = str.at(str.length() - 2);
        if (penultimo == 'I') penultimo = 'A';
        str = penultimo;
    }
    scale[5] = onDialChange(value, str + "/div");

    ui->label_7->setText(scaleText);
}
void MainWindow::on_comboBox_7_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrG6 = listaDePonteiros.at(index);
        ptrId[6] = index;
    } else {
        ptrG6 = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}
void MainWindow::on_dial_8_valueChanged(int value)
{
    QString str;
    if (ptrG7 != nullptr){
        str = listaExtendida.at(ptrId[7]);
        QChar penultimo = str.at(str.length() - 2);
        if (penultimo == 'I') penultimo = 'A';
        str = penultimo;
    }
    scale[6] = onDialChange(value, str + "/div");

    ui->label_6->setText(scaleText);
}
void MainWindow::on_comboBox_8_currentIndexChanged(int index)
{
    // Verifica se o índice é válido
    if (index >= 0 && index < listaDePonteiros.size()) {
        ptrG7 = listaDePonteiros.at(index);
        ptrId[7] = index;
    } else {
        ptrG7 = nullptr; // Índice inválido, define ptrTr como nullptr
    }
}

void MainWindow::on_checkBox_2_toggled(bool checked)
{
    ui->customPlot->graph(0)->setVisible(checked);
}
void MainWindow::on_checkBox_3_toggled(bool checked)
{
    ui->customPlot->graph(1)->setVisible(checked);
}
void MainWindow::on_checkBox_4_toggled(bool checked)
{
    ui->customPlot->graph(2)->setVisible(checked);
}
void MainWindow::on_checkBox_5_toggled(bool checked)
{
    ui->customPlot->graph(3)->setVisible(checked);
}
void MainWindow::on_checkBox_6_toggled(bool checked)
{
    ui->customPlot->graph(4)->setVisible(checked);
}
void MainWindow::on_checkBox_7_toggled(bool checked)
{
    ui->customPlot->graph(5)->setVisible(checked);
}
void MainWindow::on_checkBox_8_toggled(bool checked)
{
    ui->customPlot->graph(6)->setVisible(checked);
}

void MainWindow::on_dial_9_valueChanged(int value)
{
    trigger = ((float)value)/500;
}

void MainWindow::on_actionCheck_Devices_triggered()
{
    QString msgloc;
    pcap_if_t *devices;
    pcap_if_t *device;
    char errbuf[PCAP_ERRBUF_SIZE];
    char source[] = "rpcap://";

    /* Get the ethernet device list */
    if (pcap_findalldevs_ex(source, NULL, &devices, errbuf) == -1)
    {
        onUpdateUI(QString("pcap_findalldevs_ex: %1").arg(errbuf));
        return;
    }
    int index = 0;
    /* Iterate over the device list */
    for (device = devices; device != NULL; device = device->next)
    {
        // Use 'device' to access information about the current interface
        msgloc.append(QString("Interface Name: %1 \n").arg(device->name));
        msgloc.append(QString("Interface Description: %1 \n").arg(device->description));
        msgloc.append(QString("Interface Index: %1 \n\n").arg(index));

        index++;
    }
    pcap_freealldevs(devices);

    QMessageBox::information(this, tr("Título da mensagem"), msgloc);
}
void MainWindow::salvarTexto(const QString &texto)
{
    eht = texto;
}
void MainWindow::on_actionSet_svID_triggered()
{
    QDialog *janela1 = new QDialog(this);
    janela1->setObjectName("svID definition");
    janela1->setWindowTitle("svID definition");
    janela1->resize(366, 183);

    QVBoxLayout *layout = new QVBoxLayout(janela1);

    QLabel *label = new QLabel("Digite o svID:", janela1);
    layout->addWidget(label);

    QLineEdit *lineEdit = new QLineEdit(janela1);
    lineEdit->setText(svidref);
    layout->addWidget(lineEdit);

    // Adicionando entrada numérica float
    QLabel *floatLabel = new QLabel("Digite o Multiplicador de Tensão do SV:", janela1);
    layout->addWidget(floatLabel);

    QDoubleSpinBox *doubleSpinBox = new QDoubleSpinBox(janela1);
    doubleSpinBox->setDecimals(2); // Define a quantidade de casas decimais
    doubleSpinBox->setMinimum(-1000000); // Defina o mínimo desejado, -1000 neste exemplo
    doubleSpinBox->setMaximum(1000000); // Defina o máximo desejado, 1000 neste exemplo
    doubleSpinBox->setValue(1.0/svMultiplyT);
    layout->addWidget(doubleSpinBox);

    QLabel *floatLabel2 = new QLabel("Digite o Multiplicador de Corrente do SV:", janela1);
    layout->addWidget(floatLabel2);

    QDoubleSpinBox *doubleSpinBox2 = new QDoubleSpinBox(janela1);
    doubleSpinBox2->setDecimals(2); // Define a quantidade de casas decimais
    doubleSpinBox2->setMinimum(-1000000); // Defina o mínimo desejado, -1000 neste exemplo
    doubleSpinBox2->setMaximum(1000000); // Defina o máximo desejado, 1000 neste exemplo
    doubleSpinBox2->setValue(1.0/svMultiplyC);
    layout->addWidget(doubleSpinBox2);

    QLabel *ethLabel = new QLabel("Digite o valor de eth:", janela1);
    layout->addWidget(ethLabel);

    QLineEdit *ethLineEdit = new QLineEdit(janela1);
    ethLineEdit->setText(eht);
    layout->addWidget(ethLineEdit);

    // Adicionando caixa de seleção
    QLabel *comboBoxLabel = new QLabel("Selecione o tipo de entrada:", janela1);
    layout->addWidget(comboBoxLabel);

    QComboBox *comboBox = new QComboBox(janela1);
    comboBox->addItem("int");
    comboBox->addItem("float");
    comboBox->setCurrentIndex(svReadtype);
    layout->addWidget(comboBox);

    QPushButton *okButton = new QPushButton("OK", janela1);
    QPushButton *cancelButton = new QPushButton("Cancelar", janela1);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    // Conectar o sinal de clique do botão OK para aceitar o diálogo
    connect(okButton, &QPushButton::clicked, [this, lineEdit, doubleSpinBox, doubleSpinBox2, ethLineEdit, comboBox, janela1]() {
        svidref = lineEdit->text();
        svMultiplyT = 1.0/doubleSpinBox->value();
        svMultiplyC = 1.0/doubleSpinBox2->value();
        svReadtype = comboBox->currentIndex();
        eht = ethLineEdit->text(); // Salvando o valor de eth
        this->ui->statusbar->showMessage(QString("svID: %1. Multp.T: %2. Multp.T: %3. Tipo: %4. eth: %5")
                                             .arg(svidref).arg(doubleSpinBox->value()).arg(doubleSpinBox2->value())
                                             .arg(comboBox->itemText(svReadtype)).arg(eht));
        janela1->accept();
    });

    connect(cancelButton, &QPushButton::clicked, [janela1]() {
        janela1->close();
    });

    janela1->show();

}
/////////////////////////////////////////////////////////////////////////////
/// \brief ClientThread::ClientThread
/// \param parent
///
ClientThread::ClientThread(QObject *parent) : QThread(parent)
{
    // Inicialize membros ou faça outras configurações necessárias
}
void ClientThread::run()
{
    SVReceiver receiver = SVReceiver_create();

    emit updateUI("Thread iniciada. Using interface: " + eht);

    //SVReceiver_disableDestAddrCheck(receiver);
    SVReceiver_setInterfaceId(receiver, eht.toUtf8().constData());
    /* Create a subscriber listening to SV messages with APPID 4000h */
    SVSubscriber subscriber = SVSubscriber_create(NULL, 0x4000);
    /* Install a callback handler for the subscriber */
    SVSubscriber_setListener(subscriber, svUpdateListener, this);
    /* Connect the subscriber to the receiver */
    SVReceiver_addSubscriber(receiver, subscriber);
    /* Start listening to SV messages - starts a new receiver background thread */
    SVReceiver_start(receiver);

    if (SVReceiver_isRunning(receiver)) {
        emit updateUI("Receiver is running and listening for messages.");
        int count1s = 0;
        while (!isInterruptionRequested())
        {
            QThread::msleep(200);

            if (count1s >= 5){
                if (lossPacket > 0){
                    emit updateUI(QString("Numero de pacotes perdidos no ultimo segundo: %1").arg(lossPacket));
                    lossPacket = 0;
                }
                count1s = 0;
            }
            count1s++;
        }
    }
    else {
        emit updateUI("Failed to start SV subscriber. "
                      "Reason can be that the Ethernet"
                      " interface doesn't exist or root"
                      " permissions are required.");
    }
    emit updateUI("Encerrando a Thread...");

    SVReceiver_stop(receiver);
    SVReceiver_removeSubscriber(receiver, subscriber);
    SVReceiver_destroy(receiver);

    QThread::msleep(10);
}
void ClientThread::svUpdateListener(SVSubscriber subscriber, void *parameter, SVSubscriber_ASDU asdu)
{
    static int lastcount = 0;
    static QStringList svIdList;
    int count = 0;

    ClientThread *instance = static_cast<ClientThread*>(parameter);
    QVector<float> vetor;
    vetor.resize(8);
    QString svID = QString::fromUtf8(SVSubscriber_ASDU_getSvId(asdu));

    if(!svIdList.contains(svID))
    {
        svIdList << svID;
        emit instance->updateUI(QString("svID=(%1) smpCnt: %2 confRev: %3").arg(svID)
                                    .arg(SVSubscriber_ASDU_getSmpCnt(asdu)).arg(SVSubscriber_ASDU_getConfRev(asdu)));
        //emit instance->updateUI(QString("Sample Rate: %1\n").arg(SVSubscriber_ASDU_getSmpRate(asdu)));
        //emit instance->updateUI(QString("Size in Bytes: %1\n").arg(SVSubscriber_ASDU_getDataSize(asdu)));
        if(svIdList.size() < 32) emit instance->updateSvIdList(svIdList);
    }

    count = SVSubscriber_ASDU_getSmpCnt(asdu);
    if (count > lastcount + 1){
        instance->lossPacket = instance->lossPacket + count - (lastcount + 1);
    }
    lastcount = count;

    if(svReadtype == 1){
        vetor[0] = SVSubscriber_ASDU_getFLOAT32(asdu, 0);
        vetor[1] = SVSubscriber_ASDU_getFLOAT32(asdu, 8);
        vetor[2] = SVSubscriber_ASDU_getFLOAT32(asdu, 16);
        vetor[3] = SVSubscriber_ASDU_getFLOAT32(asdu, 24);
        vetor[4] = SVSubscriber_ASDU_getFLOAT32(asdu, 32);
        vetor[5] = SVSubscriber_ASDU_getFLOAT32(asdu, 40);
        vetor[6] = SVSubscriber_ASDU_getFLOAT32(asdu, 48);
        vetor[7] = SVSubscriber_ASDU_getFLOAT32(asdu, 56);
    }else{
        vetor[0] = (float)SVSubscriber_ASDU_getINT32(asdu, 0) * svMultiplyC;
        vetor[1] = (float)SVSubscriber_ASDU_getINT32(asdu, 8) * svMultiplyC;
        vetor[2] = (float)SVSubscriber_ASDU_getINT32(asdu, 16) * svMultiplyC;
        vetor[3] = (float)SVSubscriber_ASDU_getINT32(asdu, 24) * svMultiplyC;
        vetor[4] = (float)SVSubscriber_ASDU_getINT32(asdu, 32) * svMultiplyT;
        vetor[5] = (float)SVSubscriber_ASDU_getINT32(asdu, 40) * svMultiplyT;
        vetor[6] = (float)SVSubscriber_ASDU_getINT32(asdu, 48) * svMultiplyT;
        vetor[7] = (float)SVSubscriber_ASDU_getINT32(asdu, 56) * svMultiplyT;
    }

    Timestamp tempo;
    nsSinceEpoch tmp = SVSubscriber_ASDU_getRefrTmAsNs(asdu);

    if (SVSubscriber_ASDU_getDataSize(asdu) > 64) {
        tempo = SVSubscriber_ASDU_getTimestamp(asdu, 64);
    }else{
        Timestamp_setTimeInNanoseconds(&tempo, Hal_getTimeInNs());
    }

    if(tmp != 0) Timestamp_setTimeInNanoseconds(&tempo, tmp);

    emit instance->updateData(vetor, tempo, svIdList.indexOf(svID));

}














#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTextEdit>
#include <QDebug>
#include <QTimer>
#include <QThread>
#include <QNetworkInterface>
#include <QElapsedTimer>
#include <QTextStream>
#include <QFile>
#include <QtMath>
#include <QProcess>

#define HAVE_REMOTE

#include <pcap.h>
#include <pcap-bpf.h>
#include <pcap-stdinc.h>

#include "sv_subscriber.h"

#define cN 8
#define hN 8


// Enum para representar os índices
enum Index { Ia, Ib, Ic, In , Va, Vb, Vc, Vn};


// Defina o tipo para suas solicitações de vetores
using RequestType = QVector<QVector<float>>;



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

/////////////////////////////////////////////////////////////////////////////
/// \brief The ClientThread class
///
class ClientThread : public QThread
{
    Q_OBJECT
signals:
    void updateUI(const QString msg);
    void updateData(const QVector<float>& vetor, const Timestamp &tempo, int index);
    void updateSvIdList(const QStringList list);
public:
    ClientThread(QObject *parent = nullptr);
    void run() override;
private slots:
    static void svUpdateListener(SVSubscriber subscriber, void* parameter, SVSubscriber_ASDU asdu);
public:
    bool running;
    QString eht;
    int lossPacket = 0;
};

/////////////////////////////////////////////////////////////////////////////
/// \brief The MainWindow class
///
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void restoreData();
    void graficSetting();

    // Função para criar um QVector<QVector<float>> dinamicamente
    QVector<QVector<float>> criarData() {
        const int linhas = 8;
        const int colunas = 16777216;

        QVector<QVector<float>> data(linhas); // Cria um QVector com 8 elementos

        // Inicializa cada elemento do QVector com outro QVector<float> de tamanho colunas
        for (int i = 0; i < linhas; ++i) {
            data[i] = QVector<float>(colunas);
        }

        return data;
    }
public slots:
    void onUpdateUI(const QString msg);
    void onUpdateData(const QVector<float>& vetor, const Timestamp &tempo, int index);
    void updatePlot();
    void updateSvList(const QStringList list);

private slots:
    void salvarTexto(const QString &texto);

    void onThreadDestroyed(QObject *obj);
    void on_actionCheck_Devices_triggered();


    void on_actionSet_svID_triggered();

    void on_actionStart_triggered();


    float onDialChange(int value, QString type) ;
    void on_dial_2_valueChanged(int value);
    void on_dial_3_valueChanged(int value);
    void on_dial_4_valueChanged(int value);
    void on_dial_valueChanged(int value);
    void on_dial_5_valueChanged(int value);
    void on_dial_6_valueChanged(int value);
    void on_dial_7_valueChanged(int value);
    void on_dial_8_valueChanged(int value);

    void on_checkBox_2_toggled(bool checked);
    void on_checkBox_3_toggled(bool checked);
    void on_checkBox_4_toggled(bool checked);
    void on_checkBox_5_toggled(bool checked);
    void on_checkBox_6_toggled(bool checked);
    void on_checkBox_7_toggled(bool checked);
    void on_checkBox_8_toggled(bool checked);
    void on_actionShow_Messages_triggered();

    void on_dial_9_valueChanged(int value);

    void on_comboBox_2_currentIndexChanged(int index);

    void on_comboBox_currentIndexChanged(int index);

    void on_comboBox_3_currentIndexChanged(int index);

    void on_comboBox_4_currentIndexChanged(int index);

    void on_comboBox_5_currentIndexChanged(int index);

    void on_comboBox_6_currentIndexChanged(int index);

    void on_comboBox_7_currentIndexChanged(int index);

    void on_comboBox_8_currentIndexChanged(int index);

public:
    Ui::MainWindow *ui;
    ClientThread *clientThread = nullptr;
    QString eht = "3";
    QTimer *plotTimer;    // Timer para atualização do gráfico
    QTimer *fftTimer;

    QList<RequestType> listaDeData;
    QVector<QVector<float>> data; //dados de tensao e corrente
    QVector<quint64> dataTime; //respectivos tempos

    QVector<float>* ptrG1 = nullptr;
    QVector<float>* ptrG2 = nullptr;
    QVector<float>* ptrG3 = nullptr;
    QVector<float>* ptrG4 = nullptr;
    QVector<float>* ptrG5 = nullptr;
    QVector<float>* ptrG6 = nullptr;
    QVector<float>* ptrG7 = nullptr;
    QVector<float>* ptrTr = nullptr;


private:
    qint64 pv[32] = {0};
    int SRate = 0;
    double freq = 0;
    double stepTime = 0;
    int dialvalue[7] = {1,1,1,1,1,1,1};
    QString scaleText;
    QString msgapp;
    QStringList listadeSV;
    QList<QString> listaExtendida;
    QList<QVector<float> *> listaDePonteiros;
    float trigger = 0.0;
    int ptrId[8];
    int ptrindTr = 0;
    QStringList IndexStr = {"Ia", "Ib", "Ic", "In", "Va", "Vb", "Vc", "Vn"};

};
#endif // MAINWINDOW_H

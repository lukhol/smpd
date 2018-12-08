#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <fstream>
#include <QCheckBox>
#include <QFileDialog>
#include <QtCore>
#include <QtGui>
#include <QMessageBox>


#include"database.h"
#include"classifier.h"
#include"classifiernm.h"
#include"featuresselector.h"
#include"crossvalidation.h"
#include"classifiernn.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    bool loadFile(const std::string &fileName);
    void updateDatabaseInfo();
    void saveFile(const std::string &fileName);

    void FSupdateButtonState(void);
    void CupdateButtonState(void);

    void CsetButtonState(bool state);
    void FSsetButtonState(bool state);


    //My function:
    void prepareClassifiers();
    void selectQuality(int n);
    void fisherForNFeatures(int n);

private slots:
    void on_FSpushButtonOpenFile_clicked();

    void on_FSpushButtonCompute_clicked();

    void on_FSpushButtonSaveFile_clicked();

    void on_PpushButtonSelectFolder_clicked();


    void on_CpushButtonOpenFile_clicked();

    void on_CpushButtonSaveFile_clicked();

    void on_CpushButtonTrain_clicked();

    void on_CpushButtonExecute_clicked();


    void on_BCopenfile_clicked();

    void on_BCcrosvalidation_clicked();

    void on_BCbootstrap_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

private:
     Database database;
     Classifier* classifier;
     ClassifierNM* classifierNM;
     ClassifierNN* classifierNN;
     FeaturesSelector* featuresSelector;
     CrossValidation* crossValidation;

     std::vector<Object> training;
     std::vector<Object> test;

     void NN();
     void kNN();
     void NM();
     void NM2();
     void SFS(unsigned int n);
     void crossValidate(int k);
     void globalOpenFile();
     void featureQualityPreapreInfo();
};

#endif // MAINWINDOW_H

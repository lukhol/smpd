#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QImage>
#include <QDebug>
#include <iostream>
#include <algorithm>
#include "enums.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    FSupdateButtonState();
    CupdateButtonState();
    prepareClassifiers();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::prepareClassifiers()
{
    //ui->CcomboBoxClassifiers->addItem("NN - Nearest neighbours");
    ui->CcomboBoxClassifiers->addItem("kNN - k Nearest neighbours");
    ui->CcomboBoxClassifiers->addItem("NM - Nearest mean");
    ui->CplainTextEditTrainingPart->appendPlainText("0.8");
    for(int i = 0 ; i<12 ; i++){
        if((i+1)%2 != 0) ui->CcomboBoxK->addItem(QString::number(i+1));
    }
}

void MainWindow::updateDatabaseInfo()
{
    ui->FScomboBox->clear();
    for(unsigned int i=1; i<=database.getNoFeatures(); ++i)
        ui->FScomboBox->addItem(QString::number(i));

    ui->FStextBrowserDatabaseInfo->setText("noClass: " +  QString::number(database.getNoClass()));
    ui->FStextBrowserDatabaseInfo->append("noObjects: "  +  QString::number(database.getNoObjects()));
    ui->FStextBrowserDatabaseInfo->append("noFeatures: "  +  QString::number(database.getNoFeatures()));
}

void MainWindow::FSupdateButtonState(void)
{
    if(database.getNoObjects()==0)
    {
        FSsetButtonState(false);
    }
    else
        FSsetButtonState(true);
}

void MainWindow::CupdateButtonState(void)
{
    if(database.getNoObjects() == 0)  CsetButtonState(false);
    else CsetButtonState(true);
}

void MainWindow::FSsetButtonState(bool state)
{
   ui->FScomboBox->setEnabled(state);
   ui->FSpushButtonCompute->setEnabled(state);
   ui->FSpushButtonSaveFile->setEnabled(state);
   ui->FSradioButtonFisher->setEnabled(state);
   ui->FSradioButtonSFS->setEnabled(state);
}

void MainWindow::CsetButtonState(bool state){
    ui->CpushButtonExecute->setEnabled(state);
    ui->CpushButtonTrain->setEnabled(state);
    ui->CpushButtonSaveFile->setEnabled(state);
}

void MainWindow::on_FSpushButtonOpenFile_clicked()
{
    globalOpenFile();
}

void MainWindow::globalOpenFile(){
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open TextFile"), "", tr("Texts Files (*.txt)"));

    if ( !database.load(fileName.toStdString()) )
        QMessageBox::warning(this, "Warning", "File corrupted !");
    else
        QMessageBox::information(this, fileName, "File loaded !");


    FSupdateButtonState();
    CupdateButtonState();
    updateDatabaseInfo();
    featureQualityPreapreInfo();
}

void MainWindow::on_FSpushButtonCompute_clicked(){
    //Wyczyść pole tekstowe i od nowa wypisz:
    ui->FStextBrowserDatabaseInfo->setText("");
    ui->FStextBrowserDatabaseInfo->setText("noClass: " +  QString::number(database.getNoClass()));
    ui->FStextBrowserDatabaseInfo->append("noObjects: "  +  QString::number(database.getNoObjects()));
    ui->FStextBrowserDatabaseInfo->append("noFeatures: "  +  QString::number(database.getNoFeatures()));

    //Ile cech ma zostać sprawdzonych
    int dimension = ui->FScomboBox->currentText().toInt();

    //Jeżeli w bazie są 2 klasy to odpowiednio policz Fishera lub SFS
    if (database.getNoClass() == 2){
        //Fisher:
        if( ui->FSradioButtonFisher ->isChecked()){
             ui->FStextBrowserDatabaseInfo->append("\nFisher dla pojedynczych cech:");
             selectQuality(dimension);

             if(dimension > 1){
                 ui->FStextBrowserDatabaseInfo->append("\nFisher dla kombinacji cech:");
                 fisherForNFeatures(dimension);
             }
        //SFS:
        } else if(ui->FSradioButtonSFS->isChecked()){
            SFS(dimension);
        }
    }

}

void MainWindow::on_FSpushButtonSaveFile_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,
    tr("Open TextFile"), "D:\\Users\\Krzysiu\\Documents\\Visual Studio 2015\\Projects\\SMPD\\SMPD\\Debug\\", tr("Texts Files (*.txt)"));

        QMessageBox::information(this, "My File", fileName);
        database.save(fileName.toStdString());
}

void MainWindow::on_PpushButtonSelectFolder_clicked()
{

}

void MainWindow::on_CpushButtonOpenFile_clicked()
{
    globalOpenFile();
}

void MainWindow::on_CpushButtonSaveFile_clicked()
{

}

void MainWindow::on_CpushButtonTrain_clicked()
{
    /*
    QString tempCommunique = "";

    QString whatToDo = ui->CcomboBoxClassifiers->currentText();
    int k = ui->CcomboBoxK->currentText().toInt();
    QString trainingPart = ui->CplainTextEditTrainingPart->toPlainText();

    tempCommunique += whatToDo + "\nk=" + QString::number(k) + "\nTraining part=" + trainingPart;
    QMessageBox::warning(this, "Warning", tempCommunique);
    */

    //Sprawdzamy czy użytkownik dobrze podał podział procentowy bazy na część treningową i testową.
    QString trainingPart = ui->CplainTextEditTrainingPart->toPlainText();
    float trainingPercent = trainingPart.toFloat();
    if(!(trainingPercent > 0 && trainingPercent < 1)){
       QMessageBox::warning(this, "Warning", "Podana liczba musi być z przedziału (0-1), "
                                             "a separatorem dziesiętnym jest kropka. "
                                             "Podana liczba jest procentową wielkością bazy treningowej.");
       return;
    }

    if(classifier != nullptr) delete classifier;
    if(classifierNM != nullptr) delete classifierNM;

    //Utworzenie wektora (kopii tymczasowej) wszystkich obiektów w bazie:
    std::vector<Object> temp = database.getObjects();

    //Czyszczenie wektorów treningowych oraz testowych w przypadku ponownego mieszania bazy podstawowej:
    test.clear();
    training.clear();

    //Wielkości każdej z baz:
    int size = temp.size();
    int trainSize = size * trainingPercent;
    int testSize = size - trainSize;

    //Mieszanie wektora temp
    std::random_shuffle(temp.begin(), temp.end());

    //Wypełnienie bazy treningowej i testowej
    for(int i = 0 ; i < size ; i++){
        if(i < trainSize) training.push_back(temp[i]);
        else test.push_back(temp[i]);
    }

    //Utworzenie nowego klasyfikatora
    classifier = new Classifier(training);
    classifierNM = new ClassifierNM(training, database);
    //Test wrzucenia całej bazy:
    //classifierNM = new ClassifierNM(database.getObjects(), database);

    //Log wyświetlany w okienku komunikatów (alternatywny sposób wyświetlania został zakomentowany):
    //QString log = "\n=====================\n";
    QString log = "";
    ui->CtextBrowser->clear();

    int percent1 = static_cast<int>(trainingPercent*100),
        percent2 = static_cast<int>(100-percent1);

    log += "Baza została pomieszana funkcją shuffle, a natępnie podzielona na bazę treningową"
                  " oraz testową w stosunku " + QString::number(percent1)
                  + "% do " + QString::number(percent2) + "%."
                  "\n\n";

    log += "Ilość próbek ogólnie: " + QString::number(size) + "\n";
    log += "Ilość próbek w bazie treningowej: " + QString::number(trainSize) + "\n";
    log += "Ilość próbek w bazie testowej: " + QString::number(testSize) + "\n";

    ui->CtextBrowser->append(log);
}

void MainWindow::on_CpushButtonExecute_clicked()
{
    QString optionFromComboBox = ui->CcomboBoxClassifiers->currentText();
    if(optionFromComboBox == "NN - Nearest neighbours") NN();
    else if(optionFromComboBox == "kNN - k Nearest neighbours") kNN();
    else if(optionFromComboBox == "NM - Nearest mean") NM2();
}

void MainWindow::NN(){

}

void MainWindow::kNN(){
    if(classifierNN == nullptr) classifierNN = new ClassifierNN(database);

    int k = ui->CcomboBoxK->currentText().toInt();

    QString log ="";
    log += "kNN, k = " + QString::number(k) + " - Procentowo poprawnie: " + QString::number(classifierNN->classify(training, test, k));

    ui->CtextBrowser->append(log);
}

void MainWindow::NM(){
    /* To było na poczatku wiec moze być źle/inaczej wykonane
     * Ogólnie w klasyfikatorze (tylko raz):
     * 1.Wewnątrz klasifykatora dzielimy zbiór treningowy na tyle wektorów obiektów ile jest klas(na podsatwie klasy).
     * 2.Liczymy średnią z cech dla obiektów klasy A i klasy B (wyjdzie ich tyle ile jest cech)
     * 3.Po tym mamy wektor dla klasy A (64elementy ze średnimi z cech) oraz wektor dla klasy B (64elementy ze średnimi z cech)
     *
     * 1.Wysyłamy obiekt do sprawdzenia do klasy klasyfikatora.
     * 2.Liczymy odległość(tutaj mamy punkt 64wymiarowy średnich oraz 64 cechy obiektu) między badanym obiektem, a średnimi cechami danej klasy
     * 3.Sprawdzamy która odległość jest najmeniejsza
     */
    int howManyEquals = 0, testSize = test.size();
    QString log = "";

    for(int i = 0 ; i<test.size() ; i++){
        Object tempObject = test[i];
        std::string originalClassName = tempObject.getClassName();
        std::string fromNNClassName = classifier->NMWhichClass(test[i], database).toStdString();
        if(originalClassName == fromNNClassName) howManyEquals++;
    }

    float percentOfEquals = float(howManyEquals)/(float)testSize;

    log += "NM - Procentowo poprawnie: " + QString::number(percentOfEquals);

    ui->CtextBrowser->append(log);
}

void MainWindow::NM2(){
    classifierNM->prepareForNM();

    int howManyEquals = 0, testSize = test.size();
    QString log = "";

    for(int i = 0 ; i<test.size() ; i++){
        Object tempObject = test[i];
        std::string originalClassName = tempObject.getClassName();
        std::string fromNNClassName = classifierNM->whichClass(tempObject).toStdString();
        if(originalClassName == fromNNClassName) howManyEquals++;
    }

    float percentOfEquals = float(howManyEquals)/(float)testSize;

    log += "NM2 - Procentowo poprawnie: " + QString::number(percentOfEquals);

    ui->CtextBrowser->append(log);

}

void MainWindow::selectQuality(int n){
    if(featuresSelector != nullptr) delete featuresSelector;
    featuresSelector = new FeaturesSelector(database.getObjects(), database);

    std::vector<std::pair<float, int> >temp = featuresSelector->calculateFisherAndGetQualities(n);

    for(int i = 0 ; i < n ; i ++){
       ui->FStextBrowserDatabaseInfo->append("id cechy: "  +  QString::number(temp.at(i).second) + " " + QString::number(temp.at(i).first));
    }
}

void MainWindow::fisherForNFeatures(int n){
    if(featuresSelector != nullptr) delete featuresSelector;
    featuresSelector = new FeaturesSelector(database.getObjects(), database);
    QString log = "";
    std::vector<unsigned int> indexesFromSfsAsUnsignedInt;
    if(n>1){
        std::pair<float, std::vector<int> > bestFisher = featuresSelector->findBestFeaturesCombinationByFisher(n);
        log += "Fisher: " + QString::number(bestFisher.first) + " - ";
        for(int i = 0 ; i < bestFisher.second.size() ; i++){
            log += QString::number(bestFisher.second.at(i)) + " ";
            indexesFromSfsAsUnsignedInt.push_back((unsigned int)bestFisher.second.at(i));
        }
        database.updateIndexes(indexesFromSfsAsUnsignedInt);
        updateDatabaseInfo();
        ui->FStextBrowserDatabaseInfo->append(log);
    }
}

void MainWindow::SFS(unsigned int n){
    if(featuresSelector != nullptr) delete featuresSelector;
    featuresSelector = new FeaturesSelector(database.getObjects(), database);
    std::vector<unsigned int> indexesFromSfsAsUnsignedInt;
    QString qstring = "\nFisher: ";
    if(n>1){
        std::pair<float, std::vector<int> > fromSFS = featuresSelector->SFS(n);
        qstring += QString::number(fromSFS.first);
        qstring += ", indeksy: ";
        for(int i = 0 ; i < fromSFS.second.size() ; i++){
            qstring += QString::number(fromSFS.second.at(i)) + " ";
        }
        for(int a = 0 ; a < fromSFS.second.size() ; a++){
            indexesFromSfsAsUnsignedInt.push_back((unsigned int)fromSFS.second.at(a));
        }
    } else {
        ui->FStextBrowserDatabaseInfo->append("\nSFS - wybrano tylko jedna ceche");
        selectQuality(n);
    }
    //Redukacja cech w bazie danych po sfs
    database.updateIndexes(indexesFromSfsAsUnsignedInt);
    updateDatabaseInfo();
    ui->FStextBrowserDatabaseInfo->append(qstring);
}

void MainWindow::on_BCopenfile_clicked()
{
    globalOpenFile();
}

void MainWindow::featureQualityPreapreInfo(){
    ui->BCcomboBoxK->clear();
    int databaseSize = database.getNoObjects();

    ui->BCcomboBoxK->clear();
    ui->BCcomboBoxClassifierK->clear();
    ui->BCcomboBoxClassifier->clear();

    for(int i = 1 ; i < (databaseSize/2) + 1 ; i++){
        if(databaseSize%i == 0) {
            ui->BCcomboBoxK->addItem(QString::number(i));
        }
    }

    ui->BCcomboBoxClassifier->addItem("kNN - k Nearest neighbours");
    for(int i = 1 ; i < database.getObjects().at(0).getFeatures().size() + 1 ; i++){
        if(i%3 == 0 || i == 1){
            ui->BCcomboBoxClassifierK->addItem(QString::number(i));
        }
    }
}

void MainWindow::on_BCcrosvalidation_clicked()
{
    QString a = ui->BCcomboBoxK->currentText();
    crossValidate(a.toInt());
}

void MainWindow::on_BCbootstrap_clicked()
{
    QString a = ui->BCcomboBoxK->currentText();
    ui->BCtextBrowser->append("Bootstrap: not implemented yet.");
}

void MainWindow::on_pushButton_clicked()
{
    ui->BCtextBrowser->clear();
}

void MainWindow::crossValidate(int k){
    if(k >1){
        if(crossValidation != nullptr) delete crossValidation;
        crossValidation = new CrossValidation(database, k);

        ClassificationMethod methodToDo;
        QString methodString = ui->BCcomboBoxClassifier->currentText();

        if(methodString == "kNN - k Nearest neighbours"){
            methodToDo = ClassificationMethod::kNN;
        } else if (methodString == "NM - Nearest mean") {
            methodToDo = ClassificationMethod::NM;
        }

        float percent = crossValidation->run(methodToDo, ui->BCcomboBoxClassifierK->currentText().toInt());
        percent *= 100;
        ui->BCtextBrowser->append("k=" + QString::number(k) + " - " + QString::number(percent,'f', 2) + "%");
    } else {
        ui->BCtextBrowser->append("k=1 - co ty chcesz liczyc?");
    }
}


#include "widget_projectManage.h"
#include "ui_widget_projectManage.h"

widget_projectManage::widget_projectManage(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::widget_projectManage)
{
    ui->setupUi(this);
}

widget_projectManage::~widget_projectManage()
{
    delete ui;
}

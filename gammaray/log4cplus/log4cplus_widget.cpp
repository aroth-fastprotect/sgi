
#include "log4cplus_widget.h"
#include "ui_log4cplus_widget.h"

#include "searchlinecontroller.h"

#include <common/objectbroker.h>

using namespace sgi;

Log4cplusWidget::Log4cplusWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Log4cplusWidget)
    , m_stateManager(this)
{
    ui->setupUi(this);

    auto model = GammaRay::ObjectBroker::model(QStringLiteral("com.kdab.GammaRay.Log4cplusModel"));
    ui->mimeTypeView->header()->setObjectName("loggerHeader");
    ui->mimeTypeView->setDeferredResizeMode(0, QHeaderView::ResizeToContents);
    ui->mimeTypeView->setDeferredResizeMode(1, QHeaderView::ResizeToContents);
    ui->mimeTypeView->setModel(model);
    ui->mimeTypeView->sortByColumn(0, Qt::AscendingOrder);
    new GammaRay::SearchLineController(ui->searchLine, model);
}

Log4cplusWidget::~Log4cplusWidget()
{
}

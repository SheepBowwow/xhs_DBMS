#include "projectmanagewidget.h"
#include "ui_projectmanagewidget.h"


ProjectManageWidget::ProjectManageWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::ProjectManageWidget)
{
    ui->setupUi(this);

    m_menu = new QMenu();
}

void ProjectManageWidget::setLabelText(const QString &text)
{
    ui->lbl_projectName->setText(text);
    ui->lbl_projectName->adjustSize();
}


///两个查询待封装
void ProjectManageWidget::getProjectId(const QString &text)
{
    QSqlQuery query;
    QString queryString = "SELECT project_id FROM project_info WHERE project_name = :project_name";
    query.prepare(queryString);
    query.bindValue(":project_name", text);

    if (!query.exec()) {
        qDebug() << "Failed to execute query:" << query.lastError().text();
        return; // 处理查询失败情况
    }

    if (query.next()) {
        int projectId = query.value(0).toInt();
        qDebug() << "Project ID:" << projectId;
        // 将 projectId 设置给 project_id 属性
        _project_id = projectId;
    } else {
        qDebug() << "No project found with name:" << text;
        // 处理未找到项目的情况
        return;
    }

    QSqlQuery queryTeamer;
    QString queryTeamerString = "SELECT ui.user_nickname "
                                "FROM project_teamer pt "
                                "JOIN user_info ui ON pt.user_id = ui.user_id "
                                "WHERE pt.project_id = :project_id;";
    queryTeamer.prepare(queryTeamerString);
    queryTeamer.bindValue(":project_id", _project_id);

    if (!queryTeamer.exec()) {
        qDebug() << "Failed to execute query:" << queryTeamer.lastError().text();
        return; // 处理查询失败情况
    }

    // 清空菜单项，确保不重复添加
    m_menu->clear();


    // 添加查询到的菜单项
    while (queryTeamer.next()) {
        QString userNickname = queryTeamer.value(0).toString();
        // 获取菜单项列表
        auto menuActions = m_menu->actions();
        if (std::none_of(menuActions.begin(), menuActions.end(),
                         [&userNickname](QAction *action) { return action->text() == userNickname; })) {
            m_menu->addAction(userNickname);
        }
    }

    QString queryStr = "SELECT pni.note_link, pni.note_likes,"
                       " pni.note_collection, pni.note_remarks, pni.repay, pni.note_title,"
                       " bi.blogger_nickname, bi.blogger_id, bi.blogger_type, bi.blogger_homelink,"
                       " bi.blogger_fans, bi.blogger_likes, bi.blogger_noteprice, bi.blogger_videoprice,"
                       " bi.blogger_wechat FROM project_note_info pni JOIN bloggers_info bi ON pni.blogger_id = bi.blogger_id "
                       "WHERE pni.project_id = %1";

    queryStr = queryStr.arg(_project_id);

    _model = new MySqlQueryModel();

    _model->setQuery(queryStr);
    if (_model->lastError().isValid()) {
        qDebug() << "Failed to execute query:" << _model->lastError().text();
    }

    setTableHeader(_model, {"note_link", "note_likes", "note_collection",
                      "note_remarks", "repay", "note_title", "blogger_nickname",
                      "blogger_id", "blogger_type", "blogger_homelink", "blogger_fans",
                      "blogger_likes", "blogger_noteprice", "blogger_videoprice",
                      "blogger_wechat"});
    ui->table_projectManage->setModel(_model);
}


ProjectManageWidget::~ProjectManageWidget()
{
    delete m_menu;
    delete ui;
}


void ProjectManageWidget::on_btn_projectTeamer_released()
{
    m_menu->exec(ui->btn_projectTeamer->mapToGlobal(QPoint(0, ui->btn_projectTeamer->height())));
}


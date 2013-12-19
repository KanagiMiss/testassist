#include "testassistance.h"
#include <QSql>
#include <QSqlQuery>
#include <QVariant>
#include <QSqlError>
#include <QFileInfo>
#include <QDebug>
#include <QDateTime>
#include <cmath>

using namespace std;

const char *SQL_DATABASE_PATH         = "data.db";

TestAssistance *TestAssistance::pSingleton = NULL;

TestAssistance::TestAssistance()
{
    m_bIsLogin = false;
    m_objErrorInfo.nErrCode = TA_NOERROR;

    //init database
    m_objDatabase=QSqlDatabase::addDatabase("QSQLITE");
    m_objDatabase.setDatabaseName(SQL_DATABASE_PATH);

    QFileInfo checkFile(SQL_DATABASE_PATH);
    if(checkFile.isFile()){
        if(!m_objDatabase.open()){
            packErrorInfo(m_objDatabase.lastError().text(),
                          TA_ERROR_CANNOT_OPEN_DATABASE);
        }
    }
    else{
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
    }
}

void TestAssistance::packErrorInfo(const QString &strErrInfo, const int nErrCode)
{
    m_objErrorInfo.nErrCode = nErrCode;
    m_objErrorInfo.strErrInfo = strErrInfo;
}

bool TestAssistance::login(const QString &username, const QString &password)
{
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    //login
    QSqlQuery query;
    query.prepare("SELECT id,isadmin FROM userinfo WHERE username=:username AND password=:password");
    query.bindValue(":username", username);
    query.bindValue(":password", password);
    if(query.exec()){
        if(query.next()){
            m_bIsLogin = true;
            m_objUserInfo.nId = query.value(0).toInt();
            m_objUserInfo.strUserName = username;
            m_objUserInfo.strPassword = password;
            m_objUserInfo.isAdmin = (query.value(1).toInt()==1?true:false);
            m_objDatabase.close();
            return true;
        }
        else{
            packErrorInfo("Username or password is incorrect.",
                          TA_ERROR_USER_OR_PASS_INCORRECT);
            m_objDatabase.close();
            return false;
        }
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
}

void TestAssistance::TestAssistance::logout()
{
    //clear user info
    m_objUserInfo.isAdmin = false;
    m_objUserInfo.strUserName = "";
    m_objUserInfo.strPassword = "";
    m_objUserInfo.nId = 0;

    m_bIsLogin = false;

    //clear error info
    m_objErrorInfo.nErrCode = TA_NOERROR;
    m_objErrorInfo.strErrInfo = "";

    //clear paper info
    m_objPaperInfo.strCourse = "";
    m_objPaperInfo.kpoint.clear();
    m_objPaperInfo.major      = "";
    m_objPaperInfo.nGrade    = 0;
    m_objPaperInfo.fEasy     = 0;
    m_objPaperInfo.fNormal   = 0;
    m_objPaperInfo.fHard     = 0;
}

bool TestAssistance::userRegister(const QString &username, const QString password, bool isadmin, int &newId)
{
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }


    //register

    QSqlQuery query;
    query.prepare("SELECT username FROM userinfo WHERE username=:username");
    query.bindValue(":username", username);
    if(query.exec()){
        if(query.next()){//check if username existed
            packErrorInfo("Username existed.",
                          TA_ERROR_USERNAME_EXISTED);
            m_objDatabase.close();
            return false;
        }
        else{//username doesnot exist
            query.prepare("INSERT INTO userinfo VALUES(NULL, :username, :password, :isadmin)");
            query.bindValue(":username", username);
            query.bindValue(":password", password);
            query.bindValue(":isadmin", (isadmin?1:0));
            if(!query.exec()){
                packErrorInfo(query.lastError().text(),
                              TA_ERROR_SQLERROR);
                m_objDatabase.close();
                return false;
            }

            //get new id
            query.prepare("SELECT id FROM userinfo WHERE username=:username");
            query.bindValue(":username", username);
            query.exec();
            query.next();
            newId = query.value(0).toInt();
        }
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}


bool TestAssistance::extQuestionByType(QUESTIONTYPE type)
{
    //clear buffer
    questionBuffer.clear();
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    QSqlQuery query;
    QString sql("SELECT qtxt,qhard,qimage FROM questions "
                "WHERE qmajor=:major AND qcourse=:course "
                "AND qgrade=:grade AND qtype=:type AND qknowpoint in (");

    //pack qknowpoint
    for(size_t i = 0; i != m_objPaperInfo.kpoint.size(); ++i){
        sql += "'" + m_objPaperInfo.kpoint[i] + "'";
        if(i < m_objPaperInfo.kpoint.size()-1)
            sql += ",";
    }
    sql += ") ORDER BY RANDOM() limit 100";

    query.prepare(sql);
    query.bindValue(":major", m_objPaperInfo.major);
    query.bindValue(":course", m_objPaperInfo.strCourse);
    query.bindValue(":grade", m_objPaperInfo.nGrade);
    query.bindValue(":type", static_cast<int>(type));

    if(query.exec()){
        int maxNum = typeNum(type);
        int easy = floor(maxNum*m_objPaperInfo.fEasy);
        int normal = maxNum*m_objPaperInfo.fNormal;
        int hard = maxNum*m_objPaperInfo.fHard;

        //if sum is not equal to maxNum, set normal=normal+rest
        if(easy + normal + hard < maxNum)
            normal = maxNum - easy - hard;

        QUESTION q;
        int max = typeNum(type);
        while(query.next()){
            if(questionBuffer.size() >= max)
                break;
            int t = query.value(1).toInt();
            if(easy > 0 && t == TA_EASY){
                q.txt = query.value(0).toString();
                q.imgBytes = query.value(2).toByteArray();
                questionBuffer.push_back(q);
                easy--;maxNum--;
                continue;
            }
            if(normal > 0 && t == TA_NORMAL){
                q.txt = query.value(0).toString();
                q.imgBytes = query.value(2).toByteArray();
                questionBuffer.push_back(q);
                normal--;maxNum--;
                continue;
            }
            if(hard > 0 && t == TA_HARD){
                q.txt = query.value(0).toString();
                q.imgBytes = query.value(2).toByteArray();
                questionBuffer.push_back(q);
                hard--;maxNum--;
                continue;
            }
            if(maxNum > 0){
                q.txt = query.value(0).toString();
                q.imgBytes = query.value(2).toByteArray();
                questionBuffer.push_back(q);
                maxNum--;
            }
        }
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

void TestAssistance::extractQuestion()
{
    //init
    packedQuestions.judge.clear();
    packedQuestions.choice.clear();
    packedQuestions.fill.clear();
    packedQuestions.calc.clear();
    packedQuestions.ans.clear();

    if(m_objPaperInfo.qNum.judge>0){
        if(extQuestionByType(JUDGE)){
            packedQuestions.judge = questionBuffer;
        }
    }
    if(m_objPaperInfo.qNum.choice>0){
        if(extQuestionByType(CHOICE)){
            packedQuestions.choice = questionBuffer;
        }
    }
    if(m_objPaperInfo.qNum.fill>0){
        if(extQuestionByType(FILL)){
            packedQuestions.fill = questionBuffer;
        }
    }
    if(m_objPaperInfo.qNum.calc>0){
        if(extQuestionByType(CALC)){
            packedQuestions.calc = questionBuffer;
        }
    }
    if(m_objPaperInfo.qNum.ans>0){
        if(extQuestionByType(ANS)){
            packedQuestions.ans = questionBuffer;
        }
    }
}

void TestAssistance::setPaperInfo(const QString &course, int grade, const QString &major,
                                  const KPOINT &kpoint, const QNUM &num, const double &easy, const double &normal,
                                  const double &hard)
{
    m_objPaperInfo.strCourse = course;
    m_objPaperInfo.nGrade    = grade;
    m_objPaperInfo.major      = major;
    m_objPaperInfo.kpoint    = kpoint;
    m_objPaperInfo.qNum      = num;
    m_objPaperInfo.fEasy     = easy;
    m_objPaperInfo.fNormal   = normal;
    m_objPaperInfo.fHard     = hard;
}

bool TestAssistance::packAllKnowledgePoints()
{
    points.clear();
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    //pack points
    QSqlQuery query;
    query.prepare("SELECT DISTINCT qknowpoint FROM questions WHERE qmajor=:major AND qcourse=:course AND qgrade=:grade");
    query.bindValue(":major", m_objPaperInfo.major);
    query.bindValue(":course", m_objPaperInfo.strCourse);
    query.bindValue(":grade", m_objPaperInfo.nGrade);

    if(query.exec()){
        while(query.next()){
            qDebug() << query.value(0).toString();
            points.push_back(query.value(0).toString());
        }
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::packAllUserInfo()
{
    allUserInfo.clear();
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    //pack user info
    QSqlQuery query;
    query.prepare("SELECT id,username,password,isadmin FROM userinfo");

    USERINFO info;
    if(query.exec()){
        while(query.next()){
            info.nId = query.value(0).toInt();
            info.strUserName = query.value(1).toString();
            info.strPassword = query.value(2).toString();
            info.isAdmin = ((query.value(3).toInt() == 1)?true:false);
            allUserInfo.push_back(info);
        }
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::packAllQuestions()
{
    allQuestions.clear();
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    //pack questions
    QSqlQuery query;
    query.prepare("SELECT qno,qtxt,qgrade,qmajor,qcourse,qcnt,"
                  "qhard,qdate,qright,qtype,qknowpoint,qimage FROM questions");

    QUESTIONINFO info;
    if(query.exec()){
        while(query.next()){
            info.qno = query.value(0).toInt();
            info.qtxt = query.value(1).toString();
            info.qgrade = query.value(2).toInt();
            info.qmajor = query.value(3).toString();
            info.qcourse = query.value(4).toString();
            info.qcnt = query.value(5).toInt();
            info.qhard = query.value(6).toInt();
            info.qdate = query.value(7).toInt();
            info.qright = query.value(8).toInt();
            info.qtype = query.value(9).toInt();
            info.qknowpoint = query.value(10).toString();
            info.qimage = query.value(11).toByteArray();
            allQuestions.push_back(info);
        }
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::addQuestion(const QUESTIONINFO &info, int &newId)
{
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }


    QSqlQuery query;
    query.prepare("INSERT INTO questions VALUES(NULL,:qtxt,:qgrade,:qmajor,"
                  ":qcourse,:qcnt,:qhard,:qdate,:qright,:qtype,:qknowpoint,NULL)");

    query.bindValue(":qtxt", info.qtxt);
    query.bindValue(":qgrade", info.qgrade);
    query.bindValue(":qmajor", info.qmajor);
    query.bindValue(":qcourse", info.qcourse);
    query.bindValue(":qcnt", info.qcnt);
    query.bindValue(":qhard", info.qhard);
    query.bindValue(":qdate", info.qdate);
    query.bindValue(":qright", info.qright);
    query.bindValue(":qtype", info.qtype);
    query.bindValue(":qknowpoint", info.qknowpoint);

    if(!query.exec()){
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    query.prepare("SELECT qno FROM questions WHERE qtxt=:qtxt");
    query.bindValue(":qtxt", info.qtxt);
    query.exec();
    query.next();
    newId = query.value(0).toInt();
    m_objDatabase.close();
    return true;
}


bool TestAssistance::updateUserInfo(const USERINFO &info)
{
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }


    QSqlQuery query;
    query.prepare("UPDATE userinfo SET username=:username,"
                  "password=:password,isadmin=:isadmin "
                  "WHERE id=:id");

    query.bindValue(":id", info.nId);
    query.bindValue(":username", info.strUserName);
    query.bindValue(":password", info.strPassword);
    query.bindValue(":isadmin", (info.isAdmin?1:0));

    if(!query.exec()){
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::updateQuestionInfo(const QUESTIONINFO &info, bool updateImage)
{
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    //check if updateImage
    QString imgValue("");
    if(updateImage)
        imgValue = ",qimage=:qimage ";

    QSqlQuery query;
    query.prepare(QString("UPDATE questions SET qtxt=:qtxt,qgrade=:qgrade,") +
                  "qmajor=:qmajor,qcourse=:qcourse," +
                  "qcnt=:qcnt,qhard=:qhard," +
                  "qdate=:qdate,qright=:qright," +
                  "qtype=:qtype,qknowpoint=:qknowpoint " +
                  imgValue +
                  "WHERE qno=:qno");
    query.bindValue(":qno", info.qno);
    query.bindValue(":qtxt", info.qtxt);
    query.bindValue(":qgrade", info.qgrade);
    query.bindValue(":qmajor", info.qmajor);
    query.bindValue(":qcourse", info.qcourse);
    query.bindValue(":qcnt", info.qcnt);
    query.bindValue(":qhard", info.qhard);
    query.bindValue(":qdate", info.qdate);
    query.bindValue(":qright", info.qright);
    query.bindValue(":qtype", info.qtype);
    query.bindValue(":qknowpoint", info.qknowpoint);
    if(updateImage)
        query.bindValue(":qimage", info.qimage);

    if(!query.exec()){
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::removeUser(int id)
{
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }


    QSqlQuery query;
    query.prepare("DELETE FROM userinfo "
                  "WHERE id=:id");
    query.bindValue(":id", id);

    if(!query.exec()){
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::removeQuestion(int id)
{
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }


    QSqlQuery query;
    query.prepare("DELETE FROM questions "
                  "WHERE qno=:qno");
    query.bindValue(":qno", id);

    if(!query.exec()){
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::packAllMajors()
{
    allMajors.clear();
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    //pack user info
    QSqlQuery query;
    query.prepare("SELECT mname FROM majors");

    if(query.exec()){
        while(query.next())
            allMajors.push_back(query.value(0).toString());
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

bool TestAssistance::packCourses(const QString &major, int grade)
{
    courses.clear();
    //open database
    if (!m_objDatabase.open()) {
        packErrorInfo(m_objDatabase.lastError().text(),
                      TA_ERROR_NOTCONNECTED);
        return false;
    }

    //pack user info
    QSqlQuery query;
    query.prepare("SELECT cname FROM courses WHERE cgrade=:cgrade AND "
                  "cmajor=:cmajor");
    query.bindValue(":cmajor", major);
    query.bindValue(":cgrade", grade);

    if(query.exec()){
        while(query.next())
            courses.push_back(query.value(0).toString());
    }
    else{
        packErrorInfo(query.lastError().text(),
                      TA_ERROR_SQLERROR);
        m_objDatabase.close();
        return false;
    }
    m_objDatabase.close();
    return true;
}

#ifndef TESTASSISTANCE_H
#define TESTASSISTANCE_H

#include <QString>
#include <QSqlDatabase>
#include <QDateTime>
#include <vector>
#include <QByteArray>


/*
 * class      : TestAssistance
 * author     : misskanagi
 * data       : 2013/11/20
 * description: This class is a management class which provides interfaces about database access.
 *
 *              Below is a list of access to each attribute:
 *
 *              |attr name   |access    |
 *              |------------|----------|
 *              |USERINFO    |R/W       |
 *              |ERRORINFO   |R         |
 *              |PAPERINFO   |W         |
 *
 *
 *
*/




/*
 * user info struct
 */
struct USERINFO{
    int         nId;
    QString     strUserName;
    QString     strPassword;
    bool        isAdmin;
};


/*
 * error info struct
 */
struct ERRORINFO{
    QString strErrInfo;
    int         nErrCode;
};


/*
 * error info
 */
const int TA_NOERROR                        = 0;
const int TA_ERROR_NOTCONNECTED             = 1;
const int TA_ERROR_SQLERROR                 = 2;
const int TA_ERROR_USER_OR_PASS_INCORRECT   = 3;
const int TA_ERROR_INCORRECT                = 4;
const int TA_ERROR_USERNOTLOGIN             = 5;
const int TA_ERROR_CANNOT_OPEN_DATABASE     = 6;
const int TA_ERROR_USERNAME_EXISTED         = 7;

/*
 * difficulty definition
 */
const int TA_EASY    = 0;
const int TA_NORMAL  = 1;
const int TA_HARD    = 2;

/*
 * packed question
 */
struct QUESTION{
    QString txt;
    QByteArray imgBytes;
};

struct PACKEDQUESTIONS{
    std::vector<QUESTION> judge;
    std::vector<QUESTION> choice;
    std::vector<QUESTION> fill;
    std::vector<QUESTION> calc;
    std::vector<QUESTION> ans;
};

/*
 * question record info
 */
struct QUESTIONINFO{
    int         qno;
    QString     qtxt;
    int         qgrade;
    QString     qmajor;
    QString     qcourse;
    int         qcnt;
    int         qhard;
    int         qdate;
    int         qright;
    int         qtype;
    QString     qknowpoint;
    QByteArray  qimage;
};

/*
 * paper info struct
 */
struct QNUM{
    int judge;
    int choice;
    int fill;
    int calc;
    int ans;
};//the number of questions

typedef std::vector<QString> KPOINT;
struct PAPERINFO{
    //base info
    QString      strSchool;
    QString      strAuthor;
    QString      strPaperType;
    bool         bOpenOrClose;

    //critical info
    QString      strCourse;
    int          nGrade;
    QString      major;
    KPOINT       kpoint;    //knowledge point
    QNUM         qNum;      //number of every kind of questions
    double       fEasy;     //percentage of easy questions
    double       fNormal;   //percentage of normal questions
    double       fHard;     //percentage of hard questions
};

/*
 * Test assistance class
 */
class TestAssistance
{
    TestAssistance();
public:

    enum QUESTIONTYPE{
        JUDGE  = 0,
        CHOICE = 1,
        FILL   = 2,
        CALC   = 3,
        ANS    = 4
    };

    /* TestAssistance class is a singleton */

    static void init(){
        if(pSingleton == NULL)
            pSingleton = new TestAssistance();
    }


    static TestAssistance *get(){
        if(pSingleton == NULL)
            pSingleton = new TestAssistance();
        return pSingleton;
    }

    static void release(){
        if(pSingleton != NULL)
            delete pSingleton;
            pSingleton = NULL;
    }

    /**********************************************
     *  public interfaces of access to database
     **********************************************/

    /* user login interface */
    bool login(const QString &username, const QString &password);

    /* user logout interface */
    void logout();

    /*user register interface*/
    bool userRegister(const QString &username, const QString password, bool isadmin, int &newId);

    /* read questions from database (a test) */
    void extractQuestion();

    /* add a question to database */
    bool addQuestion(const QUESTIONINFO &info, int &newId);

    /* update user's information */
    bool updateUserInfo(const USERINFO &info);

    /* update question info */
    bool updateQuestionInfo(const QUESTIONINFO &info, bool updateImage = false);

    /* remove a user */
    bool removeUser(int id);

    /* remove a question */
    bool removeQuestion(int id);

    //pack info
    bool packAllUserInfo();
    bool packAllQuestions();
    bool packAllKnowledgePoints();
    bool packAllMajors();
    bool packCourses(const QString &major, int grade);

    /**************************************************
     *  public interfaces of access to member variable
     **************************************************/

    /* get attributes */

    const ERRORINFO &getErrorInfo()const{return m_objErrorInfo;}

    const USERINFO &getUserInfo()const{return m_objUserInfo;}

    const PAPERINFO &getPaperInfo()const{return m_objPaperInfo;}

    const QString &getUserName()const{return m_objUserInfo.strUserName;}

    //get packed questions extracted
    const PACKEDQUESTIONS &getQuestions()const{return packedQuestions;}
    //get all knowledge points points
    const KPOINT &getAllPoints()const{return points;}
    //get all user info
    const std::vector<USERINFO> &getAllUserInfo()const{return allUserInfo;}
    //get all questions
    const std::vector<QUESTIONINFO> &getAllQuestions()const{return allQuestions;}
    //get all majors
    const std::vector<QString> &getAllMajors()const{return allMajors;}
    /* get courses by major and grade */
    const std::vector<QString> &getCourses()const{return courses;}

    bool isUserAdmin()const{return m_objUserInfo.isAdmin;}
    bool isUserLogin()const{return m_bIsLogin;}

    /* set attributes */
    void setPaperInfo(const PAPERINFO &info){m_objPaperInfo = info;}
    void setPaperInfo(const QString &course, int grade, const QString &major,const KPOINT &kpoint,
                      const QNUM &num, const double &easy = 0.3, const double &normal = 0.4, const double &hard = 0.3);

private:
    static TestAssistance *pSingleton;

    /* private attributes */

    bool m_bIsLogin;
    ERRORINFO   m_objErrorInfo;
    USERINFO    m_objUserInfo;
    PAPERINFO   m_objPaperInfo;
    QSqlDatabase    m_objDatabase;

    //store all knowledge points in databse
    std::vector<QString> points;
    //store all questions selected
    PACKEDQUESTIONS packedQuestions;
    //store all users info
    std::vector<USERINFO> allUserInfo;
    //store all questions in database
    std::vector<QUESTIONINFO> allQuestions;
    //store all majors in database
    std::vector<QString> allMajors;
    //store courses;
    std::vector<QString> courses;

    std::vector<QUESTION> questionBuffer;

    /* private interfaces */

    bool extQuestionByType(QUESTIONTYPE type);

    bool isQuestionFull(QUESTIONTYPE type, int cnt){
        int max=0;
        if(type==JUDGE) max = m_objPaperInfo.qNum.judge;
        if(type==CHOICE) max = m_objPaperInfo.qNum.choice;
        if(type==FILL) max = m_objPaperInfo.qNum.fill;
        if(type==CALC) max = m_objPaperInfo.qNum.calc;
        if(type==ANS) max = m_objPaperInfo.qNum.ans;
        if(cnt>=max)
              return true;
        return false;
    }

    int typeNum(QUESTIONTYPE  type){
        int max=0;
        if(type==JUDGE) max = m_objPaperInfo.qNum.judge;
        if(type==CHOICE) max = m_objPaperInfo.qNum.choice;
        if(type==FILL) max = m_objPaperInfo.qNum.fill;
        if(type==CALC) max = m_objPaperInfo.qNum.calc;
        if(type==ANS) max = m_objPaperInfo.qNum.ans;
        return max;
    }


    /* package error information */
    void packErrorInfo(const QString &strErrInfo, const int nErrCode);

};

#endif // TESTASSISTANCE_H

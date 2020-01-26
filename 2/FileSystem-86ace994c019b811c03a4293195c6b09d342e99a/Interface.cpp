#include"Interface.h"
#include<iostream>
#include<string>
#include"FileSystem.h"
#include<Windows.h>
using namespace std;
void help() {
    cout << "***************************************************************************" << endl;
    cout << "* 1.mk + <string>                         �½�һ���ļ�,�ļ���Ϊ<string>   *" << endl;
    cout << "* 2.rm + <string>                         ɾ���ļ���Ϊ<string>���ļ�      *" << endl;
    cout << "* 3.rename + <string> + <string>          �������ļ�,�ڶ���string�������� *" << endl;
    cout << "* 4.cp + <string>                         �����ļ�,�ļ���Ϊ<string>       *" << endl;
    cout << "* 5.paste                                 ճ���ļ�                        *" << endl;
    cout << "* 6.read  + <string>                      ���ļ�,�ļ���Ϊ<string>       *" << endl;
    cout << "* 7.vi + <string>                         �༭�ļ�,�ļ���Ϊ<string>       *" << endl;
    cout << "* 8.mkdir + <string>                      �½�Ŀ¼,Ŀ¼��Ϊ<string>       *" << endl;
    cout << "* 9.rmdir + <string>                      ɾ��Ŀ¼,Ŀ¼��Ϊ<string>       *" << endl;
    cout << "* 10.renamedir + <string> + <string>      ������Ŀ¼,�ڶ���string�������� *" << endl;
    cout << "* 11.cpdir + <string>                     ����Ŀ¼,Ŀ¼��Ϊ<string>       *" << endl;
    cout << "* 12.pastedir                             ճ��Ŀ¼                        *" << endl;
    cout << "* 13.ls                                   ��ʾĿ¼����������              *" << endl;
    cout << "* 14.cd +<string>                         ��Ŀ¼��һ��Ŀ¼����һ��Ŀ¼  *" << endl;
    cout << "* 15.cd..                                 ������һ��Ŀ¼                  *" << endl;
    cout << "* 16.clear                                ����                            *" << endl;
    cout << "* 17.exit                                 ע��                            *" << endl;
    cout << "***************************************************************************" << endl
        << endl;

}


/*�û�ע��д���ļ�����*/
int regist(char username[30], char password[30]) {

    FILE *cfptr;//�ļ�ָ��
    if ((cfptr = fopen("users.ini", "a+")) == NULL) {
        printf("File client.txt could not be opened\n");
        fclose(cfptr);
        return 0;
    } else {
        fprintf(cfptr, "%s %s\n", username, password);
        fclose(cfptr);
        return 1;

    }
}
/*�û�����������飬�Ƿ��Ѿ�ע�ᣨ���ļ������ݱȽϣ�*/
int login(char username[30], char password[30]) {
    char user[30];
    char pass[30];
    FILE *cfptr;//�ļ�ָ��
    if ((cfptr = fopen("users.ini", "r")) == NULL) {
        printf("File client.txt could not be opened\n");
        fclose(cfptr);
        return 0;
    } else {
        while (!feof(cfptr)) {
            fscanf(cfptr, "%s%s", user, pass);
            if ((strcmp(username, user) == 0) && (strcmp(password, pass) == 0)) {
                fclose(cfptr);
                return 1;
            }
        }
    }
    fclose(cfptr);
    return 0;
}

int fsOperate(char name[30], char pass[30]) {
    FileSystem fs;
    fs.setUser(name, pass);
    while (1) {
        system("CLS");
        help();
        while (1) {
            cout << endl;
            string choice;

            fs.showPath();
            cin >> choice;
            if (choice == "mkdir")
                fs.newDir();
            else if (choice == "mk")
                fs.newFile();
            else if (choice == "rmdir")
                fs.deleteDir();
            else if (choice == "rm")
                fs.deleteFile();
            else if (choice == "renamedir")
                fs.renameDir();
            else if (choice == "rename")
                fs.renameFile();
            else if (choice == "cd")
                fs.readDir();
            else if (choice == "read")
                fs.readFile();
            else if (choice == "ls")
                fs.showCurrentDir();
            else if (choice == "cpdir")
                fs.copyDir();
            else if (choice == "cp")
                fs.copyFile();
            else if (choice == "pastedir")
                fs.pasteDir();
            else if (choice == "paste") {
                fs.pasteFile();
            } else if (choice == "vi")
                fs.edit();
            else if (choice == "cd..")
                fs.goback();
            else if (choice == "clear") {
                system("CLS");
                help();
            } else if (choice == "exit") {
                system("CLS");
                cout << "�û�: " << name << "����ע��...."
                    << endl;
                 Sleep(3);
                return 0;
            } else if (choice == "help") {
                help();
            } else if (choice == "mkdir?")
                cout << "������Ŀ¼ ����ʽΪ mkdir [dirname]" << endl;
            else if (choice == "mk?")
                cout << "�������ļ�����ʽΪ mk[filename]" << endl;
            else if (choice == "rmdir?")
                cout << "ɾ����Ŀ¼����ʽΪ rmdir [dirname]" << endl;
            else if (choice == "rm?")
                cout << "ɾ���ļ��������ʽΪ rm [filename]" << endl;
            else if (choice == "renamedir?")
                cout << "Ŀ¼������ " << endl;
            else if (choice == "rename?")
                cout << "�ļ������� " << endl;
            else if (choice == "cd?")
                cout << "����Ŀ¼����ʽΪ cd [dirname]" << endl;
            else if (choice == "read?")
                cout << "�鿴�ļ����ݣ���ʽΪ read [filename]" << endl;
            else if (choice == "ls?")
                cout << "��ʾĿ¼�����б�" << endl;
            else if (choice == "cpdir?")
                cout << "������Ŀ¼, ��ʽΪ cpDir [dirname]" << endl;
            else if (choice == "cy?")
                cout << "�����ļ�,��ʽΪ cp [filename]" << endl;
            else if (choice == "pastedir?")
                cout << "ճ���Ѹ��Ƶ�Ŀ¼" << endl;
            else if (choice == "paste?")
                cout << "ճ���Ѹ��Ƶ��ļ�" << endl;
            else if (choice == "clear?")
                cout << "����" << endl;
            else if (choice == "vi?")
                cout << "�༭�ĵ��������ʽΪ vi[filename]" << endl;
            else if (choice == "cd..?")
                cout << "���ص�ǰĿ¼����һ��Ŀ¼" << endl;
            else if (choice == "exit?")
                cout << "ע���ļ�ϵͳ" << endl;
            else
                cout << "������Ч��" << endl;
        }
    }

}
void run(){
    int choice = 0;
    char name[30], pass[30], pass1[30];
    system("CLS");
    Sleep(2);
    while (true) {
        system("CLS");
        cout << "\t\t*****************************************\t" << endl;
        cout << "\t\t*             ��  ��  ϵ  ͳ          \t*" << endl;
        cout << "\t\t*               1. ע�����û�         \t*" << endl;
        cout << "\t\t*               2. ��¼               \t*" << endl;
        cout << "\t\t*               3. ����              \t*" << endl;
        cout << "\t\t*               4. �˳�               \t*" << endl;
        cout << "\t\t*****************************************\t" << endl;
        cout << "\t\t��ѡ��: " ;
        cin >> choice;
        switch (choice) {
            /*ѡ��ע��*/
            case 1: {
                        cout << "�������û���:";
                        cin >> name;
                        cout << "����������: ";
                        cin >> pass;
                        cout << "��ȷ������: ";
                        cin >> pass1;
                        while (strcmp(pass, pass1) != 0) {
                            cout << "          --�������벻һ�£�������.." << endl;
                            cout << "����������: ";
                            cin >> pass;
                            cout << "��ȷ������: ";
                            cin >> pass1;
                        }
                        if (regist(name, pass) == 1){
                            cout << "        --ע��ɹ�.." << endl;
                             Sleep(2);
                        } 
                        else{
                            cout << "         --ע��ʧ�ܣ�������.." << endl;
                             Sleep(2);
                        }
                    }
                    break;

                    /*ѡ���¼*/
            case 2: {
                        cout << "�������û���:";
                        cin >> name;
                        cout << "����������:";
                        cin >> pass;
                        if (login(name, pass) == 1) {
                            cout << "       --��¼�ɹ�.." << endl;
                            fsOperate(name, pass);

                        } else{
                            cout << "          --��¼ʧ�ܣ������û���������.." << endl;
                             Sleep(2);

                        }}
                        break;
            case 3:{
            	      system("CLS");
                        cout << "\t\t*********************************************************" << endl;
                        cout << "\t\t*                        �����ĵ�                     \t*" << endl;
                        cout << "\t\t*   ���ʹ�ø�ϵͳ������helpʱ��ʾ��������İ����ĵ���\t*" << endl;
                        cout << "\t\t*        ����ĳ������+��ʱ��ʾ���������ʹ��˵��      \t*" << endl;
                        cout << "\t\t*********************************************************" << endl;
                        system("Pause");
            	     }break;
           
            case 4: {
                        system("CLS");
                        cout << "        --�˳��ļ�ϵͳ.." << endl;
                         Sleep(3);
                        exit(0);
                    }
                    break;
                    /*����ѡ��*/
            default:
                    
                    break;
        }
    }

}
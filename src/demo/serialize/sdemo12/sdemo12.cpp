#include "addressbook.pb.h"

#include "shared/steady_timer.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/logger_operators.h"

#include <QtCore>

#include <ctime>
#include <fstream>
#include <iostream>
#include <string>
#include <google/protobuf/util/time_util.h>

using namespace std;

using google::protobuf::util::TimeUtil;

struct Person
{
    enum PhoneType : quint32
    {
        MOBILE = 0,
        HOME   = 1,
        WORK   = 2,
    };

    QString name;
    qint32  id = {0};
    QString email;

    struct PhoneNumber
    {
        QString number;
        PhoneType type = {PhoneType::MOBILE};
    };
    QVector<PhoneNumber> phones;
};

// Our address book file is just one of these.
struct AddressBook
{
    QVector<Person> people;
};

//// This function fills in a Person message based on user input.
//void PromptForAddress(tutorial::Person* person, int id)
//{
//    //cout << "Enter person ID number: ";
//    //int id;
//    //cin >> id;
//    person->set_id(id);
//    //cin.ignore(256, '\n');

//    //cout << "Enter name: ";
//    //getline(cin, *person->mutable_name());

//    person->set_name(removeBraces(QUuid::createUuid().toByteArray().constData()));

//    //cout << "Enter email address (blank for none): ";
//    //string email;
//    //getline(cin, email);
//    //if (!email.empty()) {
//    //  person->set_email(email);
//    //}
//    person->set_email(removeBraces(QUuid::createUuid().toByteArray().constData()));

////    while (true)
////    {
////    cout << "Enter a phone number (or leave blank to finish): ";
////    string number;
////    getline(cin, number);
////    if (number.empty()) {
////    break;
////    }
////    phone_number->set_number(number);

//    for (int i = 0; i < 5; ++i)
//    {
//        tutorial::Person::PhoneNumber* phone_number = person->add_phones();
//        phone_number->set_number(removeBraces(QUuid::createUuid().toByteArray().constData()));
//        phone_number->set_type(tutorial::Person::MOBILE);
//    }

////    cout << "Is this a mobile, home, or work phone? ";
////    string type;
////    getline(cin, type);
////    if (type == "mobile") {
////      phone_number->set_type(tutorial::Person::MOBILE);
////    } else if (type == "home") {
////      phone_number->set_type(tutorial::Person::HOME);
////    } else if (type == "work") {
////      phone_number->set_type(tutorial::Person::WORK);
////    } else {
////      cout << "Unknown phone type.  Using default." << endl;
////    }
////  }
//  *person->mutable_last_updated() = TimeUtil::SecondsToTimestamp(time(NULL));
//}

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug, true);

    log_info << "Start 'Serialize Demo 12'";

    // Verify that the version of the library that we linked against is
    // compatible with the version of the headers we compiled against.
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    int sum = 0;
    steady_timer timer;
    AddressBook addressBookGen;

    // Add an address
    for (int i = 0; i < 100'000; ++i)
    {
        Person person;
        person.id = i;
        person.name  = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
        person.email = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);

        for (int j = 0; j < 5; ++j)
        {
            Person::PhoneNumber phoneNumber;
            phoneNumber.number = QUuid::createUuid().toString(QUuid::StringFormat::WithoutBraces);
            phoneNumber.type = Person::MOBILE;
            person.phones.append(phoneNumber);
        }
        addressBookGen.people.append(person);
    }
    log_info << "Generate test data: " << timer.elapsed();
    sum += timer.elapsed();

    timer.reset();

    tutorial::AddressBook address_book;
    // Add an address.
    for (int i = 0; i < addressBookGen.people.count(); ++i)
    {
        const Person& pers = addressBookGen.people[i];
        tutorial::Person* pbperson = address_book.add_people();

        pbperson->set_id(pers.id);
        pbperson->set_name(pers.name.toUtf8().constData());
        pbperson->set_email(pers.email.toUtf8().constData());

        for (int j = 0; j < pers.phones.count(); ++j)
        {
            const Person::PhoneNumber& pnum = pers.phones[j];
            tutorial::Person::PhoneNumber* phone_number = pbperson->add_phones();
            phone_number->set_number(pnum.number.toUtf8().constData());
            phone_number->set_type(tutorial::Person::MOBILE);
        }
    }
    log_debug << "Filling data to protobuf: " << timer.elapsed();
    sum += timer.elapsed();

    timer.reset();

    size_t buffSize = address_book.ByteSizeLong();
    QByteArray buff;
    buff.resize(buffSize);
    address_book.SerializeToArray((char*)buff.constData(), buffSize);

    log_debug << "Serializing: " << timer.elapsed();
    sum += timer.elapsed();

    timer.reset();

    QFile file;
    file.setFileName("/tmp/address_book.protobuf");
    file.open(QIODevice::WriteOnly);
    file.write(buff);
    file.close();

    log_debug << "Write to disk: " << timer.elapsed()
              << " (/tmp/address_book.protobuf)";
    sum += timer.elapsed();

    timer.reset();

    tutorial::AddressBook address_book2;
    address_book2.ParseFromArray((char*)buff.constData(), buffSize);

    log_info << "Deserializing: " << timer.elapsed();
    sum += timer.elapsed();

    AddressBook addressBook;

    int people_size = address_book2.people_size();
    for (int i = 0; i < people_size; ++i)
    {
        const ::tutorial::Person& pers = address_book2.people(i);

        Person person;
        person.id = pers.id();
        person.name  = QString::fromUtf8(pers.name().c_str());
        person.email = QString::fromUtf8(pers.email().c_str());

        int phones_size = pers.phones_size();
        for (int j = 0; j < phones_size; ++j)
        {
            const ::tutorial::Person_PhoneNumber& pnum = pers.phones(j);

            Person::PhoneNumber phoneNumber;
            phoneNumber.number = QString::fromUtf8(pnum.number().c_str());
            phoneNumber.type = static_cast<Person::PhoneType>(pnum.type()); // Person::MOBILE;
            person.phones.append(phoneNumber);
        }
        addressBook.people.append(person);
    }
    (void) addressBook;

    log_debug << "Filling AddressBook struct: " << timer.elapsed();
    sum += timer.elapsed();

    log_debug << "Sum: " << sum;

    // Optional:  Delete all global objects allocated by libprotobuf.
    google::protobuf::ShutdownProtobufLibrary();

    alog::stop();
    return 0;
}

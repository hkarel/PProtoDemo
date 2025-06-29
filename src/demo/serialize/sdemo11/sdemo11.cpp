/**
  Тест быстродействия механизма сериализации PProto на примере телефонной
  книжки для Protobuf.
  https://developers.google.com/protocol-buffers/docs/cpptutorial
*/

#include "shared/steady_timer.h"
#include "shared/logger/logger.h"
#include "shared/logger/format.h"
#include "shared/qt/quuidex.h"
#include "shared/qt/logger_operators.h"

#include "pproto/serialize/qbinary.h"
#include "pproto/bserialize_space.h"
#include "pproto/serialize/json.h"

#include <QtCore>

namespace pproto {
namespace data {

enum PhoneType : quint32
{
    MOBILE = 0,
    HOME   = 1,
    WORK   = 2,
};

struct Person
{
    qint32  id = {0};
    QByteArray name;
    QByteArray email;

    struct PhoneNumber
    {
        QByteArray number;
        PhoneType type = {PhoneType::MOBILE};

        DECLARE_B_SERIALIZE_FUNC

        void serialize(bserial::DataStream&);
        void deserialize(bserial::DataStream&);
    };
    QVector<PhoneNumber> phones;

    DECLARE_B_SERIALIZE_FUNC

    void serialize(bserial::DataStream&);
    void deserialize(bserial::DataStream&);
};

struct AddressBook
{
    QVector<Person> people;

    DECLARE_B_SERIALIZE_FUNC

    void serialize(bserial::DataStream&);
    void deserialize(bserial::DataStream&);
};

struct PersonJson
{
    qint32  id = {0};
    QString name;
    QString email;

    struct PhoneNumber
    {
        QString number;
        PhoneType type = {PhoneType::MOBILE};

        J_SERIALIZE_BEGIN
            J_SERIALIZE_ITEM( number )
            J_SERIALIZE_ITEM( type   )
        J_SERIALIZE_END
    };
    QVector<PhoneNumber> phones;

    J_SERIALIZE_BEGIN
        J_SERIALIZE_ITEM( name   )
        J_SERIALIZE_ITEM( id     )
        J_SERIALIZE_ITEM( email  )
        J_SERIALIZE_ITEM( phones )
    J_SERIALIZE_END
};

struct AddressBookJson
{
    QVector<PersonJson> people;
    J_SERIALIZE_ONE( people )
};

//---

void Person::PhoneNumber::toRaw(bserial::DataStream& stream) const
{
    B_SERIALIZE_V1
    stream << number;
    stream << type;
    B_SERIALIZE_RETURN
}

void Person::PhoneNumber::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect)
    stream >> number;
    stream >> type;
    B_DESERIALIZE_END
}

void Person::PhoneNumber::serialize(bserial::DataStream& stream)
{
    stream << number;
    stream << type;
}

void Person::PhoneNumber::deserialize(bserial::DataStream& stream)
{
    stream >> number;
    stream >> type;
}

void Person::toRaw(bserial::DataStream& stream) const
{
    B_SERIALIZE_V1
    stream << id;
    stream << name;
    stream << email;
    stream << phones;
    B_SERIALIZE_RETURN
}

void Person::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect)
    stream >> id;
    stream >> name;
    stream >> email;
    stream >> phones;
    B_DESERIALIZE_END
}

void Person::serialize(bserial::DataStream& stream)
{
    stream << id;
    stream << name;
    stream << email;
    stream << int(phones.count());
    for (int i = 0; i < phones.count(); ++i)
        phones[i].serialize(stream);
}

void Person::deserialize(bserial::DataStream& stream)
{
    stream >> id;
    stream >> name;
    stream >> email;

    int count;
    stream >> count;
    for (int i = 0; i < count; ++i)
    {
        PhoneNumber pnum;
        pnum.deserialize(stream);
        phones.append(pnum);
    }
}

void AddressBook::toRaw(bserial::DataStream& stream) const
{
    B_SERIALIZE_V1
    stream << people;
    B_SERIALIZE_RETURN
}

void AddressBook::fromRaw(const bserial::RawVector& vect)
{
    B_DESERIALIZE_V1(vect)
    stream >> people;
    B_DESERIALIZE_END
}

void AddressBook::serialize(bserial::DataStream& stream)
{
    stream << int(people.count());
    for (int i = 0; i < people.count(); ++i)
        people[i].serialize(stream);
}

void AddressBook::deserialize(bserial::DataStream& stream)
{
    int count;
    stream >> count;
    for (int i = 0; i < count; ++i)
    {
        Person pers;
        pers.deserialize(stream);
        people.append(pers);
    }
}

} // namespace data
} // namespace pproto

using namespace std;
using namespace pproto::data;

int main(int /*argc*/, char* /*argv*/[])
{
    alog::logger().start();
    alog::logger().addSaverStdOut(alog::Level::Debug, true);

    log_info << "Start 'Serialize Demo 11'";

    //int sum = 0;
    steady_timer timer;
    AddressBook addressBookGen;

    // Generation of addresses
    for (int i = 0; i < 100'000; ++i)
    {
        Person person;
        person.id = i;
        person.name  = QUuid::createUuid().toByteArray();
        person.email = QUuid::createUuid().toByteArray();

        for (int j = 0; j < 5; ++j)
        {
            Person::PhoneNumber phoneNumber;
            phoneNumber.number = QUuid::createUuid().toByteArray();
            phoneNumber.type = PhoneType::MOBILE;
            person.phones.append(phoneNumber);
        }
        addressBookGen.people.append(person);
    }
    log_info << "Generation of addresses: " << timer.elapsed();
    //sum += timer.elapsed();

    timer.reset();

    AddressBook addressBook;

    // Add an address
    for (int i = 0; i < 100'000; ++i)
    {
        const Person& pers = addressBookGen.people.at(i);

        Person person;
        person.id = pers.id;
        person.name  = pers.name;
        person.email = pers.email;

        for (int j = 0; j < pers.phones.count(); ++j)
        {
            const Person::PhoneNumber& pnumb = pers.phones.at(j);

            Person::PhoneNumber phoneNumber;
            phoneNumber.number = pnumb.number;
            phoneNumber.type = pnumb.type;
            person.phones.append(phoneNumber);
        }
        addressBook.people.append(person);
    }
    log_info << "Filling data to C++ struct: " << timer.elapsed();
    ////sum += timer.elapsed();

    AddressBookJson addressBookJson;

    // Add an address
    for (int i = 0; i < 100'000; ++i)
    {
        const Person& pers = addressBookGen.people.at(i);

        PersonJson person;
        person.id = pers.id;
        person.name  = QString::fromUtf8(pers.name);
        person.email = QString::fromUtf8(pers.email);

        for (int j = 0; j < pers.phones.count(); ++j)
        {
            const Person::PhoneNumber& pnumb = pers.phones.at(j);

            PersonJson::PhoneNumber phoneNumber;
            phoneNumber.number = QString::fromUtf8(pnumb.number);
            phoneNumber.type = pnumb.type;
            person.phones.append(phoneNumber);
        }
        addressBookJson.people.append(person);
    }
    log_info << "Filling data to C++ struct (json): " << timer.elapsed();


    timer.reset();

    QByteArray buff;
    { //Block for QDataStream
        bserial::DataStream stream {&buff, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        stream << addressBook;
    }
    log_info << "Serializing: " << timer.elapsed();
    //sum += timer.elapsed();

    timer.reset();

    QByteArray buff2;
    { //Block for QDataStream
        bserial::DataStream stream {&buff2, QIODevice::WriteOnly};
        STREAM_INIT(stream);
        addressBook.serialize(stream);
    }
    log_info << "Direct serializing: " << timer.elapsed();
    //sum += timer.elapsed();

    timer.reset();

    QByteArray json = addressBookJson.toJson();
    log_info << "Serializing to JSON: " << timer.elapsed();
    //sum += timer.elapsed();

    timer.reset();

    QFile file;
    file.setFileName("/tmp/address_book.pproto");
    file.open(QIODevice::WriteOnly);
    file.write(buff);
    file.close();

    log_info << "Write to disk: " << timer.elapsed()
             << " (/tmp/address_book.pproto)";
    //sum += timer.elapsed();

    timer.reset();

    file.setFileName("/tmp/address_book.direct");
    file.open(QIODevice::WriteOnly);
    file.write(buff2);
    file.close();

    log_info << "Write to disk: " << timer.elapsed()
             << " (/tmp/address_book.direct)";
    //sum += timer.elapsed();

    timer.reset();

    file.setFileName("/tmp/address_book.json");
    file.open(QIODevice::WriteOnly);
    file.write(json);
    file.close();

    log_info << "Write JSON to disk: " << timer.elapsed()
             << " (/tmp/address_book.json)";
    //sum += timer.elapsed();

    timer.reset();

    AddressBook addressBook2;
    { //Block for QDataStream
        bserial::DataStream stream {buff};
        STREAM_INIT(stream);
        stream >> addressBook2;
    }
    log_info << "Deserializing: " << timer.elapsed();
    //sum += timer.elapsed();

    timer.reset();

    AddressBook addressBookDirect;
    { //Block for QDataStream
        bserial::DataStream stream {buff2};
        STREAM_INIT(stream);
        addressBookDirect.deserialize(stream);
    }
    log_info << "Direct deserializing: " << timer.elapsed();
    //sum += timer.elapsed();

    timer.reset();

    AddressBookJson addressBookJson2;
    addressBookJson2.fromJson(json);
    (void) addressBookJson2;

    log_info << "Deserializing from JSON: " << timer.elapsed();

    //sum += timer.elapsed();
    //log_info << "Sum: " << sum;

    alog::stop();
    return 0;
}

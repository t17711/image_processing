/****************************************************************************
** Meta object code from reading C++ file 'Blur_weighed.h'
**
** Created by: The Qt Meta Object Compiler version 67 (Qt 5.7.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "../../Blur_weighed.h"
#include <QtCore/qbytearray.h>
#include <QtCore/qmetatype.h>
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'Blur_weighed.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 67
#error "This file was generated using the moc from 5.7.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
struct qt_meta_stringdata_Blur_weighed_t {
    QByteArrayData data[5];
    char stringdata0[50];
};
#define QT_MOC_LITERAL(idx, ofs, len) \
    Q_STATIC_BYTE_ARRAY_DATA_HEADER_INITIALIZER_WITH_OFFSET(len, \
    qptrdiff(offsetof(qt_meta_stringdata_Blur_weighed_t, stringdata0) + ofs \
        - idx * sizeof(QByteArrayData)) \
    )
static const qt_meta_stringdata_Blur_weighed_t qt_meta_stringdata_Blur_weighed = {
    {
QT_MOC_LITERAL(0, 0, 12), // "Blur_weighed"
QT_MOC_LITERAL(1, 13, 13), // "changeFilterW"
QT_MOC_LITERAL(2, 27, 0), // ""
QT_MOC_LITERAL(3, 28, 13), // "changeFilterH"
QT_MOC_LITERAL(4, 42, 7) // "setLock"

    },
    "Blur_weighed\0changeFilterW\0\0changeFilterH\0"
    "setLock"
};
#undef QT_MOC_LITERAL

static const uint qt_meta_data_Blur_weighed[] = {

 // content:
       7,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: name, argc, parameters, tag, flags
       1,    1,   29,    2, 0x09 /* Protected */,
       3,    1,   32,    2, 0x09 /* Protected */,
       4,    1,   35,    2, 0x09 /* Protected */,

 // slots: parameters
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,
    QMetaType::Void, QMetaType::Int,    2,

       0        // eod
};

void Blur_weighed::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Blur_weighed *_t = static_cast<Blur_weighed *>(_o);
        Q_UNUSED(_t)
        switch (_id) {
        case 0: _t->changeFilterW((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 1: _t->changeFilterH((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 2: _t->setLock((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObject Blur_weighed::staticMetaObject = {
    { &ImageFilter::staticMetaObject, qt_meta_stringdata_Blur_weighed.data,
      qt_meta_data_Blur_weighed,  qt_static_metacall, Q_NULLPTR, Q_NULLPTR}
};


const QMetaObject *Blur_weighed::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->dynamicMetaObject() : &staticMetaObject;
}

void *Blur_weighed::qt_metacast(const char *_clname)
{
    if (!_clname) return Q_NULLPTR;
    if (!strcmp(_clname, qt_meta_stringdata_Blur_weighed.stringdata0))
        return static_cast<void*>(const_cast< Blur_weighed*>(this));
    return ImageFilter::qt_metacast(_clname);
}

int Blur_weighed::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = ImageFilter::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    } else if (_c == QMetaObject::RegisterMethodArgumentMetaType) {
        if (_id < 3)
            *reinterpret_cast<int*>(_a[0]) = -1;
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
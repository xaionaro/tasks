#include "enumerations.h"

Enumerations::Enumerations()
{
}

struct Enumerations::enumeration Enumerations::get ( int enumeration_id )
{
	return this->id2enumeration[enumeration_id];
}

QHash<QString, QList<struct Enumerations::enumeration>> Enumerations::get ( Enumerations::item_type item_type )
{
	return this->itemType2enumeration[item_type];
}

void Enumerations::clear()
{
	this->id2enumeration.clear();
	this->itemType2enumeration.clear();
}

void Enumerations::add ( QJsonObject json_enumeration )
{
	struct enumeration enumeration;
	enumeration.id         = json_enumeration["id"].toInt();
	enumeration.name       = json_enumeration["name"].toString();
	enumeration.position   = json_enumeration["position"].toInt();
	QString      enumeration_fulltype = json_enumeration["type"].toString();
	const size_t fulltype_length      = enumeration_fulltype.length();

	if ( enumeration_fulltype.startsWith ( "issue_" ) ) {
		enumeration.item_type  = EIT_ISSUE;
		enumeration.field_name = enumeration_fulltype.right ( fulltype_length - ( sizeof ( "issue_" ) - 1 ) );
	} else if ( enumeration_fulltype.startsWith ( "time_entry_" ) ) {
		enumeration.item_type  = EIT_TIME_ENTRY;
		enumeration.field_name = enumeration_fulltype.right ( fulltype_length - ( sizeof ( "time_entry_" ) - 1 ) );
	} else if ( enumeration_fulltype.startsWith ( "document_" ) ) {
		enumeration.item_type  = EIT_DOCUMENT;
		enumeration.field_name = enumeration_fulltype.right ( fulltype_length - ( sizeof ( "document_" ) - 1 ) );
	} else {
		enumeration.item_type  = EIT_UNKNOWN;
		enumeration.field_name = enumeration_fulltype;
	}

	this->id2enumeration.insert ( enumeration.id, enumeration );
	this->itemType2enumeration[enumeration.item_type][enumeration.field_name].append ( enumeration );
}

void Enumerations::set ( QJsonArray json_array )
{
	this->clear();
	foreach ( const QJsonValue & val, json_array )
	this->add ( val.toObject() );
}

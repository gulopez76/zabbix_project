/*
** Zabbix
** Copyright (C) 2001-2020 Zabbix SIA
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
**/

#ifndef ZABBIX_DBCONFIG_H
#define ZABBIX_DBCONFIG_H

#ifndef ZBX_DBCONFIG_IMPL
#	error This header must be used by configuration cache implementation
#endif

typedef struct
{
	zbx_uint64_t	triggerid;
	const char	*description;
	const char	*expression;
	/* cached expression with expanded user macros, can be NULL */
	const char	*expression_ex;
	const char	*error;
	int		lastchange;
	unsigned char	topoindex;
	unsigned char	priority;
	unsigned char	type;
	unsigned char	value;
	unsigned char	state;
	unsigned char	locked;
	unsigned char	status;
	unsigned char	functional;	/* see TRIGGER_FUNCTIONAL_* defines */
}
ZBX_DC_TRIGGER;

typedef struct zbx_dc_trigger_deplist
{
	zbx_uint64_t		triggerid;
	int			refcount;
	ZBX_DC_TRIGGER		*trigger;
	zbx_vector_ptr_t	dependencies;
}
ZBX_DC_TRIGGER_DEPLIST;

typedef struct
{
	zbx_uint64_t	functionid;
	zbx_uint64_t	triggerid;
	zbx_uint64_t	itemid;
	const char	*function;
	const char	*parameter;
	unsigned char	timer;
}
ZBX_DC_FUNCTION;

typedef struct
{
	zbx_uint64_t	itemid;
	zbx_uint64_t	hostid;
	zbx_uint64_t	interfaceid;
	zbx_uint64_t	lastlogsize;
	zbx_uint64_t	valuemapid;
	const char	*key;
	const char	*port;
	const char	*db_error;
	ZBX_DC_TRIGGER	**triggers;
	int		delay;
	int		nextcheck;
	int		lastclock;
	int		mtime;
	int		data_expected_from;
	int		history;
	unsigned char	type;
	unsigned char	data_type;
	unsigned char	value_type;
	unsigned char	poller_type;
	unsigned char	state;
	unsigned char	db_state;
	unsigned char	inventory_link;
	unsigned char	location;
	unsigned char	flags;
	unsigned char	status;
	unsigned char	unreachable;
	unsigned char	update_triggers;
}
ZBX_DC_ITEM;

typedef struct
{
	zbx_uint64_t	hostid;
	const char	*key;
	ZBX_DC_ITEM	*item_ptr;
}
ZBX_DC_ITEM_HK;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*formula;
	const char	*units;
	int		trends;
	unsigned char	delta;
	unsigned char	multiplier;
}
ZBX_DC_NUMITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*snmp_oid;
	const char	*snmp_community;
	const char	*snmpv3_securityname;
	const char	*snmpv3_authpassphrase;
	const char	*snmpv3_privpassphrase;
	const char	*snmpv3_contextname;
	unsigned char	snmpv3_securitylevel;
	unsigned char	snmpv3_authprotocol;
	unsigned char	snmpv3_privprotocol;
	unsigned char	snmp_oid_type;
}
ZBX_DC_SNMPITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*ipmi_sensor;
}
ZBX_DC_IPMIITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*delay_flex;
}
ZBX_DC_FLEXITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*trapper_hosts;
}
ZBX_DC_TRAPITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*logtimefmt;
}
ZBX_DC_LOGITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*params;
	const char	*username;
	const char	*password;
}
ZBX_DC_DBITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*username;
	const char	*publickey;
	const char	*privatekey;
	const char	*password;
	const char	*params;
	unsigned char	authtype;
}
ZBX_DC_SSHITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*username;
	const char	*password;
	const char	*params;
}
ZBX_DC_TELNETITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*username;
	const char	*password;
}
ZBX_DC_SIMPLEITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*username;
	const char	*password;
}
ZBX_DC_JMXITEM;

typedef struct
{
	zbx_uint64_t	itemid;
	const char	*params;
}
ZBX_DC_CALCITEM;

typedef zbx_item_history_value_t	ZBX_DC_DELTAITEM;

#if defined(HAVE_POLARSSL) || defined(HAVE_GNUTLS) || defined(HAVE_OPENSSL)
typedef struct
{
	const char	*tls_psk_identity;	/* pre-shared key identity           */
	const char	*tls_psk;		/* pre-shared key value (hex-string) */
	unsigned int	refcount;		/* reference count                   */
}
ZBX_DC_PSK;
#endif

typedef struct
{
	zbx_uint64_t	hostid;
	zbx_uint64_t	proxy_hostid;
	const char	*host;
	const char	*name;
	int		maintenance_from;
	int		data_expected_from;
	int		errors_from;
	int		disable_until;
	int		snmp_errors_from;
	int		snmp_disable_until;
	int		ipmi_errors_from;
	int		ipmi_disable_until;
	int		jmx_errors_from;
	int		jmx_disable_until;

	/* item statistics per interface type */
	int		items_num;
	int		snmp_items_num;
	int		ipmi_items_num;
	int		jmx_items_num;

	/* timestamp of last availability status (available/error) field change on any interface */
	int		availability_ts;

	unsigned char	maintenance_status;
	unsigned char	maintenance_type;
	unsigned char	available;
	unsigned char	snmp_available;
	unsigned char	ipmi_available;
	unsigned char	jmx_available;
	unsigned char	status;

	/* flag to reset host availability to uknown */
	unsigned char	reset_availability;

	/* flag to force update for all items */
	unsigned char	update_items;

	/* 'tls_connect' and 'tls_accept' must be respected even if encryption support is not compiled in */
	unsigned char	tls_connect;
	unsigned char	tls_accept;
#if defined(HAVE_POLARSSL) || defined(HAVE_GNUTLS) || defined(HAVE_OPENSSL)
	const char	*tls_issuer;
	const char	*tls_subject;
	ZBX_DC_PSK	*tls_dc_psk;
#endif
	const char	*error;
	const char	*snmp_error;
	const char	*ipmi_error;
	const char	*jmx_error;
}
ZBX_DC_HOST;

typedef struct
{
	zbx_uint64_t	hostid;
	unsigned char	inventory_mode;
}
ZBX_DC_HOST_INVENTORY;

typedef struct
{
	const char	*host;
	ZBX_DC_HOST	*host_ptr;
}
ZBX_DC_HOST_H;

typedef struct
{
	zbx_uint64_t	hostid;
	int		proxy_config_nextcheck;
	int		proxy_data_nextcheck;
	int		timediff;
	int		lastaccess;
	unsigned char	location;
}
ZBX_DC_PROXY;

typedef struct
{
	zbx_uint64_t	hostid;
	const char	*ipmi_username;
	const char	*ipmi_password;
	signed char	ipmi_authtype;
	unsigned char	ipmi_privilege;
}
ZBX_DC_IPMIHOST;

typedef struct
{
	zbx_uint64_t		hostid;
	zbx_vector_uint64_t	templateids;
}
ZBX_DC_HTMPL;

typedef struct
{
	zbx_uint64_t	globalmacroid;
	const char	*macro;
	const char	*context;
	const char	*value;
}
ZBX_DC_GMACRO;

typedef struct
{
	const char		*macro;
	zbx_vector_ptr_t	gmacros;
}
ZBX_DC_GMACRO_M;

typedef struct
{
	zbx_uint64_t	hostmacroid;
	zbx_uint64_t	hostid;
	const char	*macro;
	const char	*context;
	const char	*value;
}
ZBX_DC_HMACRO;

typedef struct
{
	zbx_uint64_t		hostid;
	const char		*macro;
	zbx_vector_ptr_t	hmacros;
}
ZBX_DC_HMACRO_HM;

typedef struct
{
	zbx_uint64_t	interfaceid;
	zbx_uint64_t	hostid;
	const char	*ip;
	const char	*dns;
	const char	*port;
	unsigned char	type;
	unsigned char	main;
	unsigned char	useip;
	unsigned char	bulk;
	unsigned char	max_snmp_succeed;
	unsigned char	min_snmp_fail;
}
ZBX_DC_INTERFACE;

typedef struct
{
	zbx_uint64_t		hostid;
	ZBX_DC_INTERFACE	*interface_ptr;
	unsigned char		type;
}
ZBX_DC_INTERFACE_HT;

typedef struct
{
	const char		*addr;
	zbx_vector_uint64_t	interfaceids;
}
ZBX_DC_INTERFACE_ADDR;

typedef struct
{
	zbx_uint64_t		interfaceid;
	zbx_vector_uint64_t	itemids;
}
ZBX_DC_INTERFACE_ITEM;

typedef struct
{
	const char		*name;
	zbx_vector_uint64_t	expressionids;
}
ZBX_DC_REGEXP;

typedef struct
{
	zbx_uint64_t	expressionid;
	const char	*expression;
	const char	*regexp;
	char		delimiter;
	unsigned char	type;
	unsigned char	case_sensitive;
}
ZBX_DC_EXPRESSION;

typedef struct
{
	const char	*severity_name[TRIGGER_SEVERITY_COUNT];
	zbx_uint64_t	discovery_groupid;
	int		default_inventory_mode;
	int		refresh_unsupported;
	unsigned char	snmptrap_logging;
	/* housekeeping related configuration data */
	zbx_config_hk_t	hk;
}
ZBX_DC_CONFIG_TABLE;

typedef struct
{
	zbx_uint64_t	conditionid;
	zbx_uint64_t	actionid;

	unsigned char	conditiontype;
	unsigned char	operator;
	const char	*value;
}
zbx_dc_action_condition_t;

typedef struct
{
	zbx_uint64_t		actionid;
	const char		*formula;
	unsigned char		eventsource;
	unsigned char		evaltype;
	zbx_vector_ptr_t	conditions;
}
zbx_dc_action_t;

typedef struct
{
	/* timestamp of the last host availability diff sent to sever, used only by proxies */
	int			availability_diff_ts;

	zbx_hashset_t		items;
	zbx_hashset_t		items_hk;		/* hostid, key */
	zbx_hashset_t		numitems;
	zbx_hashset_t		snmpitems;
	zbx_hashset_t		ipmiitems;
	zbx_hashset_t		flexitems;
	zbx_hashset_t		trapitems;
	zbx_hashset_t		logitems;
	zbx_hashset_t		dbitems;
	zbx_hashset_t		sshitems;
	zbx_hashset_t		telnetitems;
	zbx_hashset_t		simpleitems;
	zbx_hashset_t		jmxitems;
	zbx_hashset_t		calcitems;
	zbx_hashset_t		deltaitems;		/* history data for delta value calculations */
	zbx_hashset_t		functions;
	zbx_hashset_t		triggers;
	zbx_hashset_t		trigdeps;
	zbx_vector_ptr_t	*time_triggers;
	zbx_hashset_t		hosts;
	zbx_hashset_t		hosts_h;		/* for searching hosts by 'host' name */
	zbx_hashset_t		hosts_p;		/* for searching proxies by 'host' name */
	zbx_hashset_t		proxies;
	zbx_hashset_t		host_inventories;
	zbx_hashset_t		ipmihosts;
	zbx_hashset_t		htmpls;
	zbx_hashset_t		gmacros;
	zbx_hashset_t		gmacros_m;		/* macro */
	zbx_hashset_t		hmacros;
	zbx_hashset_t		hmacros_hm;		/* hostid, macro */
	zbx_hashset_t		interfaces;
	zbx_hashset_t		interfaces_ht;		/* hostid, type */
	zbx_hashset_t		interface_snmpaddrs;	/* addr, interfaceids for SNMP interfaces */
	zbx_hashset_t		interface_snmpitems;	/* interfaceid, itemids for SNMP trap items */
	zbx_hashset_t		regexps;
	zbx_hashset_t		expressions;
	zbx_hashset_t		actions;
	zbx_hashset_t		action_conditions;
#if defined(HAVE_POLARSSL) || defined(HAVE_GNUTLS) || defined(HAVE_OPENSSL)
	zbx_hashset_t		psks;			/* for keeping PSK-identity and PSK pairs and for searching */
							/* by PSK identity */
#endif
	zbx_binary_heap_t	queues[ZBX_POLLER_TYPE_COUNT];
	zbx_binary_heap_t	pqueue;
	zbx_vector_uint64_t	locked_lld_ruleids;	/* for keeping track of lld rules being processed */
	ZBX_DC_CONFIG_TABLE	*config;
}
ZBX_DC_CONFIG;

void	DCdump_configuration(ZBX_DC_CONFIG *config);

#endif

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
#include "common.h"
#include "log.h"
#include "zbxalgo.h"
#include "dbcache.h"

#define ZBX_DBCONFIG_IMPL
#include "dbconfig.h"

static void	DCdump_config(ZBX_DC_CONFIG *config)
{
	const char	*__function_name = "DCdump_config";

	int		i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	if (NULL == config->config)
		goto out;

	zabbix_log(LOG_LEVEL_TRACE, "refresh_unsupported:%d", config->config->refresh_unsupported);
	zabbix_log(LOG_LEVEL_TRACE, "discovery_groupid:" ZBX_FS_UI64, config->config->discovery_groupid);
	zabbix_log(LOG_LEVEL_TRACE, "snmptrap_logging:%u", config->config->snmptrap_logging);
	zabbix_log(LOG_LEVEL_TRACE, "default_inventory_mode:%d", config->config->default_inventory_mode);

	zabbix_log(LOG_LEVEL_TRACE, "severity names:");
	for (i = 0; TRIGGER_SEVERITY_COUNT > i; i++)
		zabbix_log(LOG_LEVEL_TRACE, "  %s", config->config->severity_name[i]);

	zabbix_log(LOG_LEVEL_TRACE, "housekeeping:");
	zabbix_log(LOG_LEVEL_TRACE, "  events, mode:%u period:[trigger:%d internal:%d autoreg:%d discovery:%d]",
			config->config->hk.events_mode, config->config->hk.events_trigger,
			config->config->hk.events_internal, config->config->hk.events_autoreg,
			config->config->hk.events_discovery);

	zabbix_log(LOG_LEVEL_TRACE, "  audit, mode:%u period:%d", config->config->hk.audit_mode,
			config->config->hk.audit);

	zabbix_log(LOG_LEVEL_TRACE, "  it services, mode:%u period:%d", config->config->hk.services_mode,
			config->config->hk.services);

	zabbix_log(LOG_LEVEL_TRACE, "  user sessions, mode:%u period:%d", config->config->hk.sessions_mode,
			config->config->hk.sessions);

	zabbix_log(LOG_LEVEL_TRACE, "  history, mode:%u global:%u period:%d", config->config->hk.history_mode,
			config->config->hk.history_global, config->config->hk.history);

	zabbix_log(LOG_LEVEL_TRACE, "  trends, mode:%u global:%u period:%d", config->config->hk.trends_mode,
			config->config->hk.trends_global, config->config->hk.trends);

out:
	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_hosts(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_hosts";

	ZBX_DC_HOST		*host;
	zbx_hashset_iter_t	iter;
	zbx_vector_ptr_t	index;
	int			i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->hosts, &iter);

	while (NULL != (host = (ZBX_DC_HOST *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, host);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		host = (ZBX_DC_HOST *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "hostid:" ZBX_FS_UI64 " host:'%s' name:'%s'", host->hostid, host->host,
				host->name);

		zabbix_log(LOG_LEVEL_TRACE, "  proxy_hostid:%d", host->proxy_hostid);
		zabbix_log(LOG_LEVEL_TRACE, "  data_expected_from:%d", host->data_expected_from);

		zabbix_log(LOG_LEVEL_TRACE, "  zabbix:[available:%u, errors_from:%d disable_until:%d error:'%s']",
				host->available, host->errors_from, host->disable_until, host->error);
		zabbix_log(LOG_LEVEL_TRACE, "  snmp:[available:%u, errors_from:%d disable_until:%d error:'%s']",
				host->snmp_available, host->snmp_errors_from, host->snmp_disable_until,
				host->snmp_error);
		zabbix_log(LOG_LEVEL_TRACE, "  ipmi:[available:%u, errors_from:%d disable_until:%d error:'%s']",
				host->ipmi_available, host->ipmi_errors_from, host->ipmi_disable_until,
				host->ipmi_error);
		zabbix_log(LOG_LEVEL_TRACE, "  jmx:[available:%u, errors_from:%d disable_until:%d error:'%s']",
				host->jmx_available, host->jmx_errors_from, host->jmx_disable_until, host->jmx_error);

		/* timestamp of last availability status (available/error) field change on any interface */
		zabbix_log(LOG_LEVEL_TRACE, "  availability_ts:%d", host->availability_ts);

		zabbix_log(LOG_LEVEL_TRACE, "  maintenance_status:%u maintenance_type:%u maintenance_from:%d",
				host->maintenance_status, host->maintenance_type, host->maintenance_from);

		zabbix_log(LOG_LEVEL_TRACE, "  number of items: zabbix:%d snmp:%d ipmi:%d jmx:%d", host->items_num,
				host->snmp_items_num, host->ipmi_items_num, host->jmx_items_num);

		/* 'tls_connect' and 'tls_accept' must be respected even if encryption support is not compiled in */
		zabbix_log(LOG_LEVEL_TRACE, "  tls:[connect:%u accept:%u]", host->tls_connect, host->tls_accept);
#if defined(HAVE_POLARSSL) || defined(HAVE_GNUTLS) || defined(HAVE_OPENSSL)
		zabbix_log(LOG_LEVEL_TRACE, "  tls:[issuer:'%s' subject:'%s']", host->tls_issuer, host->tls_subject);

		if (NULL != host->tls_dc_psk)
		{
			zabbix_log(LOG_LEVEL_TRACE, "  tls:[psk_identity:'%s' psk:'%s' dc_psk:%u]",
					host->tls_dc_psk->tls_psk_identity, host->tls_dc_psk->tls_psk,
					host->tls_dc_psk->refcount);
		}
#endif
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_proxies(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_proxies";

	ZBX_DC_PROXY		*proxy;
	zbx_hashset_iter_t	iter;
	zbx_vector_ptr_t	index;
	int			i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->proxies, &iter);

	while (NULL != (proxy = (ZBX_DC_PROXY *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, proxy);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		proxy = (ZBX_DC_PROXY *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "hostid:" ZBX_FS_UI64 " timediff:%d location:%u", proxy->hostid,
				proxy->timediff, proxy->location);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_ipmihosts(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_ipmihosts";

	ZBX_DC_IPMIHOST		*ipmihost;
	zbx_hashset_iter_t	iter;
	zbx_vector_ptr_t	index;
	int			i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->ipmihosts, &iter);

	while (NULL != (ipmihost = (ZBX_DC_IPMIHOST *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, ipmihost);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		ipmihost = (ZBX_DC_IPMIHOST *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "hostid:" ZBX_FS_UI64 " ipmi:[username:'%s' password:'%s' authtype:%d"
				" privilege:%u]", ipmihost->hostid, ipmihost->ipmi_username, ipmihost->ipmi_password,
				ipmihost->ipmi_authtype, ipmihost->ipmi_privilege);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_host_inventories(ZBX_DC_CONFIG *config)
{
	const char			*__function_name = "DCdump_host_inventories";

	ZBX_DC_HOST_INVENTORY		*host_inventory;
	zbx_hashset_iter_t		iter;
	zbx_vector_ptr_t		index;
	int				i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->host_inventories, &iter);

	while (NULL != (host_inventory = (ZBX_DC_HOST_INVENTORY *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, host_inventory);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		host_inventory = (ZBX_DC_HOST_INVENTORY *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "hostid:" ZBX_FS_UI64 " inventory_mode:%u", host_inventory->hostid,
				host_inventory->inventory_mode);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "  End of %s()", __function_name);
}

static void	DCdump_htmpls(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_htmpls";

	ZBX_DC_HTMPL		*htmpl = NULL;
	zbx_hashset_iter_t	iter;
	zbx_vector_ptr_t	index;
	int			i, j;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->htmpls, &iter);

	while (NULL != (htmpl = (ZBX_DC_HTMPL *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, htmpl);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		htmpl = (ZBX_DC_HTMPL *)index.values[i];

		zabbix_log(LOG_LEVEL_TRACE, "hostid:" ZBX_FS_UI64, htmpl->hostid);

		for (j = 0; j < htmpl->templateids.values_num; j++)
			zabbix_log(LOG_LEVEL_TRACE, "  templateid:" ZBX_FS_UI64, htmpl->templateids.values[j]);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_gmacros(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_gmacros";

	ZBX_DC_GMACRO		*gmacro;
	zbx_hashset_iter_t	iter;
	zbx_vector_ptr_t	index;
	int			i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->gmacros, &iter);

	while (NULL != (gmacro = (ZBX_DC_GMACRO *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, gmacro);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		gmacro = (ZBX_DC_GMACRO *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "globalmacroid:" ZBX_FS_UI64 " macro:'%s' value:'%s' context:'%s'",
				gmacro->globalmacroid, gmacro->macro,
				gmacro->value, ZBX_NULL2EMPTY_STR(gmacro->context));
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_hmacros(ZBX_DC_CONFIG *config)
{
	const char	*__function_name = "DCdump_hmacros";

	ZBX_DC_HMACRO		*hmacro;
	zbx_hashset_iter_t	iter;
	zbx_vector_ptr_t	index;
	int			i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->hmacros, &iter);

	while (NULL != (hmacro = (ZBX_DC_HMACRO *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, hmacro);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		hmacro = (ZBX_DC_HMACRO *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "hostmacroid:" ZBX_FS_UI64 " hostid:" ZBX_FS_UI64 " macro:'%s' value:'%s'"
				" context '%s'", hmacro->hostmacroid, hmacro->hostid, hmacro->macro, hmacro->value,
				ZBX_NULL2EMPTY_STR(hmacro->context));
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_interfaces(ZBX_DC_CONFIG *config)
{
	const char	*__function_name = "DCdump_interfaces";

	ZBX_DC_INTERFACE	*interface;
	zbx_hashset_iter_t	iter;
	zbx_vector_ptr_t	index;
	int			i;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->interfaces, &iter);

	while (NULL != (interface = (ZBX_DC_INTERFACE *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, interface);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		interface = (ZBX_DC_INTERFACE *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "interfaceid:" ZBX_FS_UI64 " hostid:" ZBX_FS_UI64 " ip:'%s' dns:'%s'"
				" port:'%s' type:%u main:%u useip:%u bulk:%u",
				interface->interfaceid, interface->hostid, interface->ip, interface->dns,
				interface->port, interface->type, interface->main, interface->useip, interface->bulk);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_numitem(const ZBX_DC_NUMITEM *numitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  formula:'%s' units:'%s' trends:%d delta:%u multiplier:%u", numitem->formula,
			numitem->units, numitem->trends, numitem->delta, numitem->multiplier);
}

static void	DCdump_snmpitem(const ZBX_DC_SNMPITEM *snmpitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  snmp:[oid:'%s' community:'%s' oid_type:%u]", snmpitem->snmp_oid,
			snmpitem->snmp_community, snmpitem->snmp_oid_type);

	zabbix_log(LOG_LEVEL_TRACE, "  snmpv3:[securityname:'%s' authpassphrase:'%s' privpassphrase:'%s']",
			snmpitem->snmpv3_securityname, snmpitem->snmpv3_authpassphrase,
			snmpitem->snmpv3_privpassphrase);

	zabbix_log(LOG_LEVEL_TRACE, "  snmpv3:[contextname:'%s' securitylevel:%u authprotocol:%u privprotocol:%u]",
			snmpitem->snmpv3_contextname, snmpitem->snmpv3_securitylevel, snmpitem->snmpv3_authprotocol,
			snmpitem->snmpv3_privprotocol);
}

static void	DCdump_ipmiitem(const ZBX_DC_IPMIITEM *ipmiitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  ipmi_sensor:'%s'", ipmiitem->ipmi_sensor);
}

static void	DCdump_flexitem(const ZBX_DC_FLEXITEM *flexitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  delay_flex:'%s'", flexitem->delay_flex);
}

static void	DCdump_trapitem(const ZBX_DC_TRAPITEM *trapitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  trapper_hosts:'%s'", trapitem->trapper_hosts);
}

static void	DCdump_logitem(ZBX_DC_LOGITEM *logitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  logtimefmt:'%s'", logitem->logtimefmt);
}

static void	DCdump_dbitem(const ZBX_DC_DBITEM *dbitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  db:[params:'%s' username:'%s' password:'%s']", dbitem->params,
			dbitem->username, dbitem->password);
}

static void	DCdump_sshitem(const ZBX_DC_SSHITEM *sshitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  ssh:[username:'%s' password:'%s' authtype:%u params:'%s']",
			sshitem->username, sshitem->password, sshitem->authtype, sshitem->params);
	zabbix_log(LOG_LEVEL_TRACE, "  ssh:[publickey:'%s' privatekey:'%s']", sshitem->publickey,
			sshitem->privatekey);
}

static void	DCdump_telnetitem(const ZBX_DC_TELNETITEM *telnetitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  telnet:[username:'%s' password:'%s' params:'%s']", telnetitem->username,
			telnetitem->password, telnetitem->params);
}

static void	DCdump_simpleitem(const ZBX_DC_SIMPLEITEM *simpleitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  simple:[username:'%s' password:'%s']", simpleitem->username,
			simpleitem->password);
}

static void	DCdump_jmxitem(const ZBX_DC_JMXITEM *jmxitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  jmx:[username:'%s' password:'%s']", jmxitem->username, jmxitem->password);
}

static void	DCdump_calcitem(const ZBX_DC_CALCITEM *calcitem)
{
	zabbix_log(LOG_LEVEL_TRACE, "  calc:[params:'%s']", calcitem->params);
}

/* item type specific information debug logging support */

typedef void (*zbx_dc_dump_func_t)(void *);

typedef struct
{
	zbx_hashset_t		*hashset;
	zbx_dc_dump_func_t	dump_func;
}
zbx_trace_item_t;

static void	DCdump_items(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_items";

	ZBX_DC_ITEM		*item;
	zbx_hashset_iter_t	iter;
	int			i, j;
	zbx_vector_ptr_t	index;
	void			*ptr;
	zbx_trace_item_t	trace_items[] =
	{
		{&config->numitems, (zbx_dc_dump_func_t)DCdump_numitem},
		{&config->snmpitems, (zbx_dc_dump_func_t)DCdump_snmpitem},
		{&config->ipmiitems, (zbx_dc_dump_func_t)DCdump_ipmiitem},
		{&config->flexitems, (zbx_dc_dump_func_t)DCdump_flexitem},
		{&config->trapitems, (zbx_dc_dump_func_t)DCdump_trapitem},
		{&config->logitems, (zbx_dc_dump_func_t)DCdump_logitem},
		{&config->dbitems, (zbx_dc_dump_func_t)DCdump_dbitem},
		{&config->sshitems, (zbx_dc_dump_func_t)DCdump_sshitem},
		{&config->telnetitems, (zbx_dc_dump_func_t)DCdump_telnetitem},
		{&config->simpleitems, (zbx_dc_dump_func_t)DCdump_simpleitem},
		{&config->jmxitems, (zbx_dc_dump_func_t)DCdump_jmxitem},
		{&config->calcitems, (zbx_dc_dump_func_t)DCdump_calcitem}
	};

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->items, &iter);

	while (NULL != (item = (ZBX_DC_ITEM *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, item);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		item = (ZBX_DC_ITEM *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "itemid:" ZBX_FS_UI64 " hostid:" ZBX_FS_UI64 " key:'%s'",
				item->itemid, item->hostid, item->key);
		zabbix_log(LOG_LEVEL_TRACE, "  type:%u value_type:%u data_type:%u", item->type, item->value_type,
				item->data_type);
		zabbix_log(LOG_LEVEL_TRACE, "  interfaceid:" ZBX_FS_UI64 " port:'%s'", item->interfaceid, item->port);
		zabbix_log(LOG_LEVEL_TRACE, "  state:%u db_state:%u db_error:'%s'", item->state, item->db_state,
				item->db_error);
		zabbix_log(LOG_LEVEL_TRACE, "  flags:%u status:%u", item->flags, item->status);
		zabbix_log(LOG_LEVEL_TRACE, "  valuemapid:" ZBX_FS_UI64, item->valuemapid);
		zabbix_log(LOG_LEVEL_TRACE, "  lastlogsize:" ZBX_FS_UI64 " mtime:%d", item->lastlogsize, item->mtime);
		zabbix_log(LOG_LEVEL_TRACE, "  delay:%d nextcheck:%d lastclock:%d", item->delay, item->nextcheck,
				item->lastclock);
		zabbix_log(LOG_LEVEL_TRACE, "  data_expected_from:%d", item->data_expected_from);
		zabbix_log(LOG_LEVEL_TRACE, "  history:%d", item->history);
		zabbix_log(LOG_LEVEL_TRACE, "  poller_type:%u location:%u", item->poller_type, item->location);
		zabbix_log(LOG_LEVEL_TRACE, "  inventory_link:%u", item->inventory_link);
		zabbix_log(LOG_LEVEL_TRACE, "  unreachable %u", item->unreachable);

		for (j = 0; j < (int)ARRSIZE(trace_items); j++)
		{
			if (NULL != (ptr = zbx_hashset_search(trace_items[j].hashset, &item->itemid)))
				trace_items[j].dump_func(ptr);
		}

		if (NULL != item->triggers)
		{
			ZBX_DC_TRIGGER	*trigger;

			zabbix_log(LOG_LEVEL_TRACE, "  triggers:");

			for (j = 0; NULL != (trigger = item->triggers[j]); j++)
				zabbix_log(LOG_LEVEL_TRACE, "    triggerid:" ZBX_FS_UI64, trigger->triggerid);
		}
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_interface_snmpitems(ZBX_DC_CONFIG *config)
{
	const char			*__function_name = "DCdump_interface_snmpitems";

	ZBX_DC_INTERFACE_ITEM		*interface_snmpitem;
	zbx_hashset_iter_t		iter;
	int				i, j;
	zbx_vector_ptr_t		index;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->interface_snmpitems, &iter);

	while (NULL != (interface_snmpitem = (ZBX_DC_INTERFACE_ITEM *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, interface_snmpitem);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		interface_snmpitem = (ZBX_DC_INTERFACE_ITEM *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "interfaceid:" ZBX_FS_UI64, interface_snmpitem->interfaceid);

		for (j = 0; j < interface_snmpitem->itemids.values_num; j++)
			zabbix_log(LOG_LEVEL_TRACE, "  itemid:" ZBX_FS_UI64, interface_snmpitem->itemids.values[j]);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_functions(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_functions";

	ZBX_DC_FUNCTION		*function;
	zbx_hashset_iter_t	iter;
	int			i;
	zbx_vector_ptr_t	index;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->functions, &iter);

	while (NULL != (function = (ZBX_DC_FUNCTION *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, function);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		function = (ZBX_DC_FUNCTION *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "functionid:" ZBX_FS_UI64 " triggerid:" ZBX_FS_UI64 " itemid:"
				ZBX_FS_UI64 " function:'%s' parameter:'%s' timer:%u", function->functionid,
				function->triggerid, function->itemid, function->function, function->parameter,
				function->timer);

	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_triggers(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_triggers";

	ZBX_DC_TRIGGER		*trigger;
	zbx_hashset_iter_t	iter;
	int			i;
	zbx_vector_ptr_t	index;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->triggers, &iter);

	while (NULL != (trigger = (ZBX_DC_TRIGGER *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, trigger);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		trigger = (ZBX_DC_TRIGGER *)index.values[i];

		zabbix_log(LOG_LEVEL_TRACE, "triggerid:" ZBX_FS_UI64 " description:'%s' type:%u status:%u priority:%u",
					trigger->triggerid, trigger->description, trigger->type, trigger->status,
					trigger->priority);
		zabbix_log(LOG_LEVEL_TRACE, "  expression:'%s' cached:'%s'", trigger->expression,
				ZBX_NULL2EMPTY_STR(trigger->expression_ex));
		zabbix_log(LOG_LEVEL_TRACE, "  value:%u state:%u error:'%s' lastchange:%d", trigger->value,
				trigger->state, ZBX_NULL2EMPTY_STR(trigger->error), trigger->lastchange);
		zabbix_log(LOG_LEVEL_TRACE, "  topoindex:%u functional:%u locked:%u", trigger->topoindex,
				trigger->functional, trigger->locked);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_trigdeps(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_trigdeps";

	ZBX_DC_TRIGGER_DEPLIST	*trigdep;
	zbx_hashset_iter_t	iter;
	int			i, j;
	zbx_vector_ptr_t	index;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->trigdeps, &iter);

	while (NULL != (trigdep = (ZBX_DC_TRIGGER_DEPLIST *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, trigdep);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		trigdep = (ZBX_DC_TRIGGER_DEPLIST *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "triggerid:" ZBX_FS_UI64 " refcount:%d", trigdep->triggerid,
				trigdep->refcount);

		for (j = 0; j < trigdep->dependencies.values_num; j++)
		{
			const ZBX_DC_TRIGGER_DEPLIST	*trigdep_up = trigdep->dependencies.values[j];

			zabbix_log(LOG_LEVEL_TRACE, "  triggerid:" ZBX_FS_UI64, trigdep_up->triggerid);
		}
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_expressions(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_expressions";

	ZBX_DC_EXPRESSION	*expression;
	zbx_hashset_iter_t	iter;
	int			i;
	zbx_vector_ptr_t	index;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->expressions, &iter);

	while (NULL != (expression = (ZBX_DC_EXPRESSION *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, expression);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		expression = (ZBX_DC_EXPRESSION *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "expressionid:" ZBX_FS_UI64 " regexp:'%s' expression:'%s delimiter:%d"
				" type:%u case_sensitive:%u", expression->expressionid, expression->regexp,
				expression->expression, expression->delimiter, expression->type,
				expression->case_sensitive);
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

static void	DCdump_actions(ZBX_DC_CONFIG *config)
{
	const char		*__function_name = "DCdump_actions";

	zbx_dc_action_t		*action;
	zbx_hashset_iter_t	iter;
	int			i, j;
	zbx_vector_ptr_t	index;

	zabbix_log(LOG_LEVEL_TRACE, "In %s()", __function_name);

	zbx_vector_ptr_create(&index);
	zbx_hashset_iter_reset(&config->actions, &iter);

	while (NULL != (action = (zbx_dc_action_t *)zbx_hashset_iter_next(&iter)))
		zbx_vector_ptr_append(&index, action);

	zbx_vector_ptr_sort(&index, ZBX_DEFAULT_UINT64_PTR_COMPARE_FUNC);

	for (i = 0; i < index.values_num; i++)
	{
		action = (zbx_dc_action_t *)index.values[i];
		zabbix_log(LOG_LEVEL_TRACE, "actionid:" ZBX_FS_UI64 " formula:'%s' eventsource:%u evaltype:%u",
				action->actionid, action->formula, action->eventsource, action->evaltype);

		for (j = 0; j < action->conditions.values_num; j++)
		{
			zbx_dc_action_condition_t	*condition = action->conditions.values[j];

			zabbix_log(LOG_LEVEL_TRACE, "  conditionid:" ZBX_FS_UI64 " conditiontype:%u operator:%u"
					" value:'%s'", condition->conditionid, condition->conditiontype,
					condition->operator, condition->value);
		}
	}

	zbx_vector_ptr_destroy(&index);

	zabbix_log(LOG_LEVEL_TRACE, "End of %s()", __function_name);
}

void	DCdump_configuration(ZBX_DC_CONFIG *config)
{
	DCdump_config(config);
	DCdump_hosts(config);
	DCdump_proxies(config);
	DCdump_ipmihosts(config);
	DCdump_host_inventories(config);
	DCdump_htmpls(config);
	DCdump_gmacros(config);
	DCdump_hmacros(config);
	DCdump_interfaces(config);
	DCdump_items(config);
	DCdump_interface_snmpitems(config);
	DCdump_triggers(config);
	DCdump_trigdeps(config);
	DCdump_functions(config);
	DCdump_expressions(config);
	DCdump_actions(config);
}

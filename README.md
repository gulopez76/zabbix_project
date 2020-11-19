# zabbix_project
Deploy zabbix platform from ansible

    Quiron project:
    
         Sentence to deploy server and agent: ansible-playbook -vvv -i /storage/izertis/quiron/zabbix/hosts install_zabbix_server.yml -l {servers}
         Sentence to deploy sproxy: ansible-playbook -vvv -i /storage/izertis/sas/hosts_test install_zabbix_proxy.yml -l { servers ]
         
    Sas project:
    
         Sentence to deploy proxy and agent: ansible-playbook -vvv -i hosts install_zabbix_proxy.yml -l {servers}

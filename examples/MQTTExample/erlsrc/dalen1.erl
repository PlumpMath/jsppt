-module(dalen1).
-author("Mikael Karlsson <karlsson.rm@gmail.com>").

-mod_title("dalen1 zotonic site").
-mod_description("An empty Zotonic site, to base your site on.").
-mod_prio(10).

-include_lib("zotonic.hrl").
-include_lib("emqtt/include/emqtt.hrl").


%%====================================================================
%% support functions go here
%%====================================================================

-export([
    'mqtt:~site/iot/#'/3,
    observe_acl_is_allowed/2
]).

'mqtt:~site/iot/#'(#mqtt_msg{topic=T,payload=P}, _ModulePid, Context) ->
    T1 = binary:split(T,<<"/">>, [global]),
    [ApplianceId|_] = lists:reverse(T1),
    lager:info("site/dalen1/iot/# received: ~s from device ~s", [P, ApplianceId]),
    case binary:split(P,<<":">>) of
	[<<"11">>|_] ->
	    lager:info("More than 10 networks found"),
	    A = << ": >10 networks" >>,
	    z_mqtt:publish("~site/iotnotes", << ApplianceId/binary, A/binary >>, Context);
	_ ->
	    do_nothing
    end,
    ok.

%% Allow iot to publish on iot and everybody on iotnotes
observe_acl_is_allowed(#acl_is_allowed{object=#acl_mqtt{ words=[<<"site">>, _, <<"iot">>, _ApplianceId] }},
		       #context{user_id = Id}=Context) ->
    case m_identity:get_username(Id, Context) of
	<<"iot">> ->
	    true;
	_ -> undefined
    end;
observe_acl_is_allowed(#acl_is_allowed{object=#acl_mqtt{ words=[<<"site">>, _, <<"iotnotes">>|_] }},
		       _Context) ->
    true;
observe_acl_is_allowed(_AclIsAllowed, _Context) ->
    undefined.

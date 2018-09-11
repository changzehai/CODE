// SNMP应用层程序

#include <includes.h>
#include "arch/cc.h"
#include "Snmp\snmp_SNMP.h"
#include "Snmp\snmp_mib.h"
#define ERR_VGO(s, v, t) goto t


/* Local copies of Agent Context information */
static u16_t    nmibs;
//static uint16     trapv, trapid;
static const    MIB** mibs;

extern u8_t*    Snmp_Context;

extern AGENT_CONTEXT snmp_ac;

// 初始化模块变量

void ussSNMPAgentInit( void )
{
    /* Save global context information for Agent */
    mibs = snmp_ac.mibs;
    nmibs = snmp_ac.nummibs;
    //trapv = snmp_ac.trapv;
    //trapid = 1;
}

u8_t test1[16] = {0};
u8_t test2[16] = {0};
s16_t isAccessAllowed( const u8_t* contextName )
{
    u8_t i = 0;
    for( i = 0; i < 16; i++ )
    {
        test1[i] = Snmp_Context[i];
        test2[i] = contextName[i];
    }
    if( sncompare( contextName, ( u8_t* )Snmp_Context, 16 ) == 0 )
    {
        return accessAllowed;
    }
    return noSuchContext;
}


// 处理SNMP输入，并生成应答
s16_t inReqOutRep( u8_t** obp, u16_t olen, const u8_t* ibp, u16_t ilen )
{
    const   MIB*     mibp = NULL;
    const   MIBVAR*  mvp;
    const   MIBTAB*  mtp;
    const   u8_t*    varbinds[MAXVAR];
    const   u8_t*    ibpend, *chkp, *chkp2, *inp;
    u8_t*    obpend, *outp, *valp, *cp, *soutp;
    u8_t*    contextName;
    u16_t*       emptyflag;
    u32_t   ul1, reqid, nonrep, maxrep;
    s32_t   sl1, getsetflag;
    s16_t   i1, i2, i3, len, len2, mibix, tabix, ixlen;
    u8_t    type, version, errix, errcode, nflag;
    s8_t    vindex;
    u16_t  i;
    s32_t  r;
    
    
    i3 = 0;
    errix = errcode = nflag = 0;
    /* snmp输入信息，从前往后顺序读取 */
    ibpend = ibp + ilen;                /* Read from start to end */
    inp = ibp;
	
    /* 应答信息从后往前倒着生成 */
    outp = *obp;
    obpend = *obp - olen;
	
    contextName = obpend + 1;
    *contextName = 0;

	
    /* 取出SNMP输入消息长度 */
    len = snmpReadLength( &inp, Sequence );
    if( len <= 0 )
    {
err_parse:
        return 0;
    }
	
    chkp = inp + len;   /* Check length of message */
	
    /* 取出msgVersion */
    len = snmpReadInt( &ul1, sizeof( ul1 ), &inp, Integer );
    if( len <= 0 || len > ( u8_t )sizeof( ul1 ) )
    {
        ERR_VGO( "msgVersion Length", len, err_parse );
    }
	
    version = ul1;
    /* version只限于snmp v1和snmp v2版本 */ 
    if( version != 0 && version != 1 )
    {
        ERR_VGO( "Bad version", version, err_parse );
    }

    /* 取出Community */
    len = snmpReadVal( contextName, olen, &inp, String );
    if( len < 0 || ( u16_t )len > olen )
    {
        ERR_VGO( "ContextName Length", len, err_parse );
    }
    contextName[len] = 0;           /* Null terminate contextName */

    /* 取出PDU   type */
    type = *inp;
    len = snmpReadLength( &inp, type );
    if( len <= 0 )
    {
        ERR_VGO( "Type Field Length", len, err_parse );
    }
	
    chkp2 = inp + len;

    /* 取出Request ID */
    len = snmpReadInt( &reqid, sizeof( reqid ), &inp, Integer );
    if( len < 1 )
    {
        ERR_VGO( "Request-ID Length", len, err_parse );
    }

    /* snmp v1时为Error status；snmp v2时为nonrepeaters */
    len = snmpReadInt( &nonrep, sizeof( nonrep ), &inp, Integer );
    if( len <= 0 )
    {
        ERR_VGO( "Error value Length", len, err_parse );
    }
	
    /* snmp v1时为Error index；snmp v2时为max-repetitions */
    len = snmpReadInt( &maxrep, sizeof( maxrep ), &inp, Integer );
    if( len <= 0 )
    {
        ERR_VGO( "Error Index Length", len, err_parse );
    }
	
    /* Variable bindings所占长度 */
    len = snmpReadLength( &inp, Sequence );
    if( len < 0 )
    {
        ERR_VGO( "PDU Data Length", len, err_parse );
    }
	
    /* 取出Variable bindings中所有的变量名值对 */
    vindex = 0;
    varbinds[0] = inp;
    while( inp < ibpend )
    {
        /* SEQUENCE OF { OID, Value } */
        varbinds[vindex++] = inp;   /* Save bindings */
        /* SEQUENCE */
        len2 = snmpReadLength( &inp, Sequence );
        if( len2 <= 0 )
        {
            ERR_VGO( "Variable", vindex, err_parse );
        }
        inp += len2;
        if( vindex >= MAXVAR )
        {
            errcode = tooBig;
            errix = 0;
            vindex = 0;             /* Skip variable processing */
            break;
        }
    }
	
    if( inp > ibpend )
    {
        goto err_parse;
    }
	
    if( inp != chkp )
    {
        goto err_parse;
    }
	
    nflag = 0;
    switch( type )
    {
        case SetRequest:
            getsetflag = 1;
        case GetRequest:
            getsetflag = 1;
            break;
        case GetNextRequest:
            getsetflag = 0;
            nflag = 1;
            break;
/* 2018-06-29 changzehai(DTT) ----------------------------------- ADD Start -*/
    case GetBulkRequest:
            getsetflag = 0;
            nflag = 1;
        break;
/* 2018-06-29 changzehai(DTT) ----------------------------------- ADD End   -*/            
        default:
            goto err_parse;
            //break;
    };
    
    int rrr;
    int s_vindex = vindex;
    rrr = vindex - nonrep;
    
    if (type == GetBulkRequest) 
    {
        for(i2 = 0; i2 < maxrep; i2++)
        {

        vindex = s_vindex;
        while(vindex >= rrr) 
        {
        inp = varbinds[vindex - 1];     /* Point to variable binding */
        snmpReadLength( &inp, Sequence );
        len2 = snmpReadLength( &inp, Identifier );
        if( len2 <= 0 )
        {
            ERR_VGO( "Variable OID Length", len2, err_parse );
        }
        
        mvp = 0;
        mtp = 0;
        tabix = -1;
        soutp = outp;
		
        /* 在MIB中搜索该变量 */
        for( mibix = 0; mibix < ( s16_t )nmibs && mvp == 0; mibix++ )
        {
            mibp = mibs[mibix];
            for( i = 0; i < mibp->numvars(); i++ )
            {
                mvp = &mibp->mvp[i];
                if( mvp->opt == NOACCESS )
                {
                    continue;
                }
 
				/* 比较snmp输入变OID与MIB变量OID */
                r = snmpVCompare( inp, len2, mvp->oid.name, mvp->oid.nlen );

				
                // 完全匹配
                if( r == 0 )
                {
                    /* Perfect match */
                    tabix = ( s16_t )mvp->oid.name[mvp->oid.nlen - 1] - 1;
                    if( tabix < 0 )
                    {
                        /* scalar var */
                        if( getsetflag )
                        {
                            tabix = -1;
                            break;
                        }
                    }
                    else
                    {
                        /* table var */
                        if( getsetflag == 0 )
                        {
                            sl1 = snmpFindOID( ( const u8_t** )&mtp,
                                               ( u8_t* )mibp->mtp, sizeof( MIBTAB ), mibp->numtabs(),
                                               ( u8_t* )mvp->oid.name, mvp->oid.nlen - 2 );
                            /* If not table found or not an exact match */
                            if( mtp == 0 || sl1 )
                            {
                                /* Generate a general error */
                                i1 = genErr;
                                ERR_VGO( "No matching table for var!", sl1, err_val );
                            }
                            if( mtp->empty )
                            {
                                emptyflag = ( u16_t* )( mtp->empty );
                                if( !( *emptyflag ) )
                                {
                                    mvp = 0;
                                }
                                else
                                {
                                    tabix = 0;
                                    break;      //found
                                }
                            }
                            else
                            {
                                tabix = 0;
                                break;       //found
                            }
                        }
                    }
                }
                // 包含基准
                else if( ( r > 0 ) && ( r < 255 ) )
                {
                    tabix = ( s16_t )mvp->oid.name[mvp->oid.nlen - 1] - 1;
                    if( tabix >= 0 )
                    {
                        /* Find the table using found OID */
                        sl1 = snmpFindOID( ( const u8_t** )&mtp,
                                           ( u8_t* )mibp->mtp, sizeof( MIBTAB ), mibp->numtabs(),
                                           ( u8_t* )mvp->oid.name, mvp->oid.nlen - 2 );
                        /* If not table found or not an exact match */
                        if( mtp == 0 || sl1 )
                        {
                            /* Generate a general error */
                            i1 = genErr;
                            ERR_VGO( "No matching table for var!", sl1, err_val );
                        }
                        ixlen = len2 - mvp->oid.nlen;
                        /* Find Index number into MIB table */
                        //tabix = i2;
                        tabix = maxrep - i2;
                        sl1 = snmpFindBulkIndex( &tabix, mtp, mibp, mvp,
                                             inp + mvp->oid.nlen, ( u8_t )ixlen, nflag );
                        if( sl1 )
                        {
                            break;    //found
                        }
                        else
                        {
                            mvp = 0;
                        }
                    }
                }
                // 被基准包含或比基准小
                else if( r < 0 )
                {
                    tabix = ( s16_t )mvp->oid.name[mvp->oid.nlen - 1] - 1;
                    if( tabix < 0 )
                    {
                        /* scalar var */
                        if( getsetflag )
                        {
                            mvp = 0;
                            break;
                        }
                        else
                        {
                            tabix = -1;
                            break;
                        }
                    }
                    else
                    {
                        /* table var */
                        if( getsetflag )
                        {
                            mvp = 0;
                            break;
                        }
                        else
                        {
                            sl1 = snmpFindOID( ( const u8_t** )&mtp,
                                               ( u8_t* )mibp->mtp, sizeof( MIBTAB ), mibp->numtabs(),
                                               ( u8_t* )mvp->oid.name, mvp->oid.nlen - 2 );
                            /* If not table found or not an exact match */
                            if( mtp == 0 || sl1 )
                            {
                                /* Generate a general error */
                                i1 = genErr;
                                ERR_VGO( "No matching table for var!", sl1, err_val );
                            }
                            if( mtp->empty )
                            {
                                emptyflag = ( u16_t* )( mtp->empty );
                                if( !( *emptyflag ) )
                                {
                                    mvp = 0;
                                }
                                else
                                {
                                    tabix = 0;
                                    break;      //found
                                }
                            }
                            else
                            {
                                tabix = 0;
                                break;      //found
                            }
                        }
                    }
                }
                mvp = 0;
            }
        }  
        
        /* 找不到匹配的MIB变量 */
        if( mvp == 0 )
        {
            /* If SNMPv1 */
            //            if( version == 0 )
            if( version == 1 )
                /* Generate a noSuchName error */
            {
                i1 = noSuchName;
            }
            /* Else if SNMPv2 or SNMPv3 */
            else
                /* Generate a NULL variable binding value */
            {
                i1 = 0;
            }
            ERR_VGO( "End of MIB for var", vindex, err_val1 );
        }
        
        
   //     tabix = tabix + maxrep - i2 - 1;
        valp = mvp->ptr;
        
        /* 表格变量需要偏移到变量值存储位置 */
        if( tabix >= 0 )
        {
            if( mvp->opt & SX )
#ifdef LITTLE
                valp = ( u8_t* )&tabix; /* Point to low order byte */
#else
                valp = ( u8_t* )&tabix + sizeof( tabix ) - 1;
#endif
            else if( ( ( mvp->opt & SCALAR ) == 0 ) && !( ( mvp->opt & CAR ) && mibp->get ) )
            {
                /* 偏移到索引对应的数据位置 */
                valp += mtp->len * tabix;
            }
        }
 
            /* 判断操作选项 */
        switch( mvp->opt & 7 )
        {
            case IMMED:
                ul1 = mvp->len;
                snmpRWriteInt( &outp, ul1, mvp->type, sizeof( mvp->type ) );
                break;
            case IMMED2:
                ul1 = ( ( u32_t )mvp->type << 8 ) + mvp->len;
                snmpRWriteInt( &outp, ul1, Integer, 2 );
                break;
            case BASE1:
                if( type == SetRequest )
                {
                    *valp = *chkp2 - 1;
                }
                snmpRWriteInt( &outp, ( *valp ) + 1, mvp->type, mvp->len );
                break;
            case 0:
            case SCALAR:
                cp = valp;
	        /* 判断存储类型,根据存储类型写入变量值 */
                switch( mvp->type )
                {
                    case Integer:
                    case Counter:
                    case Ticks:
                    case Gauge:
                        if( mvp->opt & SX )
                        {
                            ul1 = tabix + 1;
                        }
                        else
                        {
                            ul1 = 0;
#ifdef LITTLE
                            if( ( mvp->opt & NWORDER ) == 0 )
                                for( i1 = mvp->len; i1--; )
                                {
                                    ul1 = ( ul1 << 8 ) + cp[i1];
                                }
                            else
#endif
                                for( i1 = mvp->len; i1; i1-- )
                                {
                                    ul1 = ( ul1 << 8 ) + *cp++;
                                }
                        }
                        snmpRWriteInt( &outp, ul1, mvp->type, mvp->len );
                        break;
                    case String:
                        /* Add one level of indirection for strings */
                        //cp = *(uint8 **)valp;
                        cp = ( u8_t* )valp;
			/* 写入变量值的类型、长度、内容 */
			/* 应答消息buffer的内容变为: [...][变量值类型][变量值长度][变量值内容] */
                        snmpRWriteVal( &outp, cp, String, slength( cp ) );
                        break;
                    default:
                        if( mvp->type == OctetString )
                        {
                            i2 = mvp->type;
                            snmpRWriteVal( &outp, valp, String, mvp->len );
                            break;
                        }
                        i2 = mvp->type;
                        snmpRWriteVal( &outp, valp, i2, mvp->len );
                        break;
                }
                break;
            default:
                i1 = genErr;
                ERR_VGO( "Invalid options for var", vindex, err_val1 );
        }

        if( i1 < 0 )
        {
            ERR_VGO( "MIB set() failed", i1, err_val1 );
err_val1:
            /* Version 2/3 doesn't set errcode for noSuchName */
            if( !version || i1 != noSuchName )
            {
                errcode = i1;   /* Saved from before goto! */
                errix = vindex;
            }
            snmpRWriteVal( &outp, 0, version ? ( nflag ? 0x82 : 0x81 ) : Null, 0 );
            snmpRWriteVal( &outp, inp, Identifier, len2 );  /* Input ID */
            mvp = 0;
        }
        else
        {
         
            if( tabix < 0 ) /* 标量 */
            {
                /*
                 * 将OID及OID长度写入应答buffer中。
                 * 应答消息buffer的内容变为:[...][变量OID类型][变量OID长度][变量OID][变量值类型][变量值长度][变量值内容]
                 */
                snmpRWriteVal( &outp, mvp->oid.name, Identifier, mvp->oid.nlen );
            }
            else /* 表格变量 */
            {
                chkp = outp;

		/*
		 * 将索引值编码,并写入应答buffer中                                                        
		 * 应答消息buffer的内容变为:[...][索引值][变量值类型][变量值长度][变量值内容]
		 */
                snmpEncodeIndex( &outp, mibp, mtp, tabix, mvp );

		/*
		 * 将应答变量的OID及OID长度写入应答buffer中
		 * 应答消息buffer的内容变为:[...][变量OID类型][变量OID长度][变量OID][索引值][变量值类型][变量值长度][变量值内容] 
		 */
                snmpRWriteVal( &outp, mvp->oid.name, Identifier, mvp->oid.nlen );
				
		/*
		 * 移动指针到OID长度处 
		 */
                snmpReadLength( ( const u8_t** )&outp, Identifier ); 

		/*
                 * 修改OID长度，新的长度 = OID长度 + 索引值长度 
	         * 应答消息buffer的内容变为:[...][变量OID类型][变量OID长度(新)][变量OID][索引值][变量值类型][变量值长度][变量值内容]
	         */
                snmpRWriteLength( &outp, Identifier, chkp - outp );
            }
        }


		/*
		 * 写入该应答变量的类型及其长度。
		 * 应答消息buffer的内容变为:[...][名值对x编码类型][名值对x总长度][变量OID类型][变量OID长度(新)][变量OID][索引值][变量值类型][变量值长度][变量值内容]
                 * 自此，一个名值对的内容写入完毕，如果存在多个名值对,则
                 *  应答消息buffer的内容简化为：[...]<名值对1><名值对2><名值对3><名值对x...>
                 */
		snmpRWriteLength( &outp, Sequence, soutp - outp );
		
        /* 前一个变量 */
        vindex--;
    }
        }
        
    }
    
    
    		
    /* 从后往前处理每一个变量 */
    while( vindex )
    {
        inp = varbinds[vindex - 1];     /* Point to variable binding */
        snmpReadLength( &inp, Sequence );
        len2 = snmpReadLength( &inp, Identifier );
        if( len2 <= 0 )
        {
            ERR_VGO( "Variable OID Length", len2, err_parse );
        }
        
        mvp = 0;
        mtp = 0;
        tabix = -1;
        soutp = outp;
		
        /* 在MIB中搜索该变量 */
        for( mibix = 0; mibix < ( s16_t )nmibs && mvp == 0; mibix++ )
        {
            mibp = mibs[mibix];
            for( i = 0; i < mibp->numvars(); i++ )
            {
                mvp = &mibp->mvp[i];
                if( mvp->opt == NOACCESS )
                {
                    continue;
                }
 
				/* 比较snmp输入变OID与MIB变量OID */
                r = snmpVCompare( inp, len2, mvp->oid.name, mvp->oid.nlen );

				
                // 完全匹配
                if( r == 0 )
                {
                    /* Perfect match */
                    tabix = ( s16_t )mvp->oid.name[mvp->oid.nlen - 1] - 1;
                    if( tabix < 0 )
                    {
                        /* scalar var */
                        if( getsetflag )
                        {
                            tabix = -1;
                            break;
                        }
                    }
                    else
                    {
                        /* table var */
                        if( getsetflag == 0 )
                        {
                            sl1 = snmpFindOID( ( const u8_t** )&mtp,
                                               ( u8_t* )mibp->mtp, sizeof( MIBTAB ), mibp->numtabs(),
                                               ( u8_t* )mvp->oid.name, mvp->oid.nlen - 2 );
                            /* If not table found or not an exact match */
                            if( mtp == 0 || sl1 )
                            {
                                /* Generate a general error */
                                i1 = genErr;
                                ERR_VGO( "No matching table for var!", sl1, err_val );
                            }
                            if( mtp->empty )
                            {
                                emptyflag = ( u16_t* )( mtp->empty );
                                if( !( *emptyflag ) )
                                {
                                    mvp = 0;
                                }
                                else
                                {
                                    tabix = 0;
                                    break;      //found
                                }
                            }
                            else
                            {
                                tabix = 0;
                                break;       //found
                            }
                        }
                    }
                }
                // 包含基准
                else if( ( r > 0 ) && ( r < 255 ) )
                {
                    tabix = ( s16_t )mvp->oid.name[mvp->oid.nlen - 1] - 1;
                    if( tabix >= 0 )
                    {
                        /* Find the table using found OID */
                        sl1 = snmpFindOID( ( const u8_t** )&mtp,
                                           ( u8_t* )mibp->mtp, sizeof( MIBTAB ), mibp->numtabs(),
                                           ( u8_t* )mvp->oid.name, mvp->oid.nlen - 2 );
                        /* If not table found or not an exact match */
                        if( mtp == 0 || sl1 )
                        {
                            /* Generate a general error */
                            i1 = genErr;
                            ERR_VGO( "No matching table for var!", sl1, err_val );
                        }
                        ixlen = len2 - mvp->oid.nlen;
                        /* Find Index number into MIB table */
                        sl1 = snmpFindIndex( &tabix, mtp, mibp, mvp,
                                             inp + mvp->oid.nlen, ( u8_t )ixlen, nflag );
                        if( sl1 )
                        {
                            break;    //found
                        }
                        else
                        {
                            mvp = 0;
                        }
                    }
                }
                // 被基准包含或比基准小
                else if( r < 0 )
                {
                    tabix = ( s16_t )mvp->oid.name[mvp->oid.nlen - 1] - 1;
                    if( tabix < 0 )
                    {
                        /* scalar var */
                        if( getsetflag )
                        {
                            mvp = 0;
                            break;
                        }
                        else
                        {
                            tabix = -1;
                            break;
                        }
                    }
                    else
                    {
                        /* table var */
                        if( getsetflag )
                        {
                            mvp = 0;
                            break;
                        }
                        else
                        {
                            sl1 = snmpFindOID( ( const u8_t** )&mtp,
                                               ( u8_t* )mibp->mtp, sizeof( MIBTAB ), mibp->numtabs(),
                                               ( u8_t* )mvp->oid.name, mvp->oid.nlen - 2 );
                            /* If not table found or not an exact match */
                            if( mtp == 0 || sl1 )
                            {
                                /* Generate a general error */
                                i1 = genErr;
                                ERR_VGO( "No matching table for var!", sl1, err_val );
                            }
                            if( mtp->empty )
                            {
                                emptyflag = ( u16_t* )( mtp->empty );
                                if( !( *emptyflag ) )
                                {
                                    mvp = 0;
                                }
                                else
                                {
                                    tabix = 0;
                                    break;      //found
                                }
                            }
                            else
                            {
                                tabix = 0;
                                break;      //found
                            }
                        }
                    }
                }
                mvp = 0;
            }
        }
		
		
		/* 找不到匹配的MIB变量 */
        if( mvp == 0 )
        {
            /* If SNMPv1 */
            //            if( version == 0 )
            if( version == 1 )
                /* Generate a noSuchName error */
            {
                i1 = noSuchName;
            }
            /* Else if SNMPv2 or SNMPv3 */
            else
                /* Generate a NULL variable binding value */
            {
                i1 = 0;
            }
            ERR_VGO( "End of MIB for var", vindex, err_val );
        }


		/* 检查Community是否匹配 */
        i1 = isAccessAllowed( contextName );
        switch( i1 )
        {
            case accessAllowed:
                break;
            case noSuchContext:
            default:
                i1 = noAccess;
                ERR_VGO( "VACM failure for var", vindex, err_val );
        }
		
        valp = mvp->ptr;
		
        /* 检查长度字段 */
        if( type == SetRequest )
        {
            inp += len2;    /* Point to data field */
            chkp2 = inp;
			
            if( mvp->type == OctetString )
            {
                len = snmpReadLength( ( const u8_t** )&chkp2, String );
            }
            else
            {
                len = snmpReadLength( ( const u8_t** )&chkp2, mvp->type );
            }
            if( len < 0 )
            {
                ERR_VGO( "Bad length field for var", vindex, err_parse );
            }
            i1 = 0;
            if( ( mvp->opt & W ) == 0 )
            {
                i1 = readOnly;
            }
            else if( mvp->type == Integer || mvp->type == Counter ||
                     mvp->type == Ticks || mvp->type == Gauge ||
                     mvp->type == OctetString )
            {
                if( len > mvp->len || len < 1 )
                {
                    i1 = wrongLength;
                }
                if( mvp->type == Integer && *chkp2 & 0x80 )
                {
                    i3 = 0xff;    /* Value is negative */
                }
                else
                {
                    i3 = 0;    /* Value is positive */
                }
            }
            else if( mvp->type == String )
            {
                if( len >= mvp->len )
                {
                    i1 = wrongLength;
                }
            }
            else if( len != mvp->len )
            {
                i1 = wrongLength;
            }
            if( i1 )
            {
                ERR_VGO( "Incorrect length for SetRequest", vindex, err_val );
            }
            else if( mibp->check )
            {
                i1 = mibp->check( mvp - mibp->mvp, tabix, inp );
                if( i1 )
                {
                    ERR_VGO( "MIB check invalidated SetRequest", vindex, err_val );
                }
            }
        }
        else
        {
            /* Perform read notification if possible and necessary */
            if( ( mvp->opt & CAR ) && mibp->get )
            {
                mibp->get( mvp - mibp->mvp, tabix, &valp );
            }
        }
		

        /* 表格变量需要偏移到变量值存储位置 */
        if( tabix >= 0 )
        {
            if( mvp->opt & SX )
#ifdef LITTLE
                valp = ( u8_t* )&tabix; /* Point to low order byte */
#else
                valp = ( u8_t* )&tabix + sizeof( tabix ) - 1;
#endif
            else if( ( ( mvp->opt & SCALAR ) == 0 ) && !( ( mvp->opt & CAR ) && mibp->get ) )
            {
                /* 偏移到索引对应的数据位置 */
                valp += mtp->len * tabix;
            }
        }
		
        /* 判断操作选项 */
        switch( mvp->opt & 7 )
        {
            case IMMED:
                ul1 = mvp->len;
                snmpRWriteInt( &outp, ul1, mvp->type, sizeof( mvp->type ) );
                break;
            case IMMED2:
                ul1 = ( ( u32_t )mvp->type << 8 ) + mvp->len;
                snmpRWriteInt( &outp, ul1, Integer, 2 );
                break;
            case BASE1:
                if( type == SetRequest )
                {
                    *valp = *chkp2 - 1;
                }
                snmpRWriteInt( &outp, ( *valp ) + 1, mvp->type, mvp->len );
                break;
            case 0:
            case SCALAR:
                cp = valp;
	        /* 判断存储类型,根据存储类型写入变量值 */
                switch( mvp->type )
                {
                    case Integer:
                    case Counter:
                    case Ticks:
                    case Gauge:
                        if( type == SetRequest )
                        {
                            len = snmpReadInt( &ul1, mvp->len, &inp, mvp->type );
                            if( len < 1 )
                            {
                                ERR_VGO( "Bad type/length field for var", vindex, err_parse );
                            }
                            i2 = ( mvp->len > len ) ? len : mvp->len;
                            i1 = mvp->len - i2;
#ifdef LITTLE
                            if( ( mvp->opt & NWORDER ) == 0 )
                            {
                                /* Fill low order bytes with value */
                                while( i2-- )
                                {
                                    *cp++ = ul1;
                                    ul1 >>= 8;
                                }
                                /* Fill high order bytes with sign-ness */
                                while( i1-- )
                                {
                                    *cp++ = ( u8_t )i3;
                                }
                            }
                            else
#endif
                            {
                                cp += mvp->len;
                                /* Fill low order bytes with value */
                                while( i2-- )
                                {
                                    *--cp = ul1;
                                    ul1 >>= 8;
                                }
                                /* Fill high order bytes with sign-ness */
                                while( i1-- )
                                {
                                    *--cp = ( u8_t )i3;
                                }
                            }
                            cp = valp;
                        }
                        if( mvp->opt & SX )
                        {
                            ul1 = tabix + 1;
                        }
                        else
                        {
                            ul1 = 0;
#ifdef LITTLE
                            if( ( mvp->opt & NWORDER ) == 0 )
                                for( i1 = mvp->len; i1--; )
                                {
                                    ul1 = ( ul1 << 8 ) + cp[i1];
                                }
                            else
#endif
                                for( i1 = mvp->len; i1; i1-- )
                                {
                                    ul1 = ( ul1 << 8 ) + *cp++;
                                }
                        }
                        snmpRWriteInt( &outp, ul1, mvp->type, mvp->len );
                        break;
                    case String:
                        /* Add one level of indirection for strings */
                        //cp = *(uint8 **)valp;
                        cp = ( u8_t* )valp;
                        if( type == SetRequest )
                        {
                            len = snmpReadVal( cp, mvp->len, &inp, mvp->type );
                            if( len < 0 || len > mvp->len )
                            {
                                ERR_VGO( "Bad type/length field for var", vindex, err_parse );
                            }
                            cp[len] = 0;
                        }
			/* 写入变量值的类型、长度、内容 */
			/* 应答消息buffer的内容变为: [...][变量值类型][变量值长度][变量值内容] */
                        snmpRWriteVal( &outp, cp, String, slength( cp ) );
                        break;
                    default:
                        if( mvp->type == OctetString )
                        {
                            i2 = mvp->type;
                            if( type == SetRequest )
                            {
                                len = snmpReadVal( cp, mvp->len, &inp, String );
                                if( len < 0 || len > mvp->len )
                                {
                                    ERR_VGO( "Cannot set var", vindex, err_parse );
                                }
                            }
                            snmpRWriteVal( &outp, valp, String, mvp->len );
                            break;
                        }
                        i2 = mvp->type;
                        if( type == SetRequest )
                        {
                            len = snmpReadVal( cp, mvp->len, &inp, i2 );
                            if( len < 0 || len > mvp->len )
                            {
                                ERR_VGO( "Cannot set var", vindex, err_parse );
                            }
                        }
                        snmpRWriteVal( &outp, valp, i2, mvp->len );
                        break;
                }
                break;
            default:
                i1 = genErr;
                ERR_VGO( "Invalid options for var", vindex, err_val );
        }
		
        /* Perform write notification if possible and necessary */
        if( type == SetRequest && mvp->opt & CAW && mibp->set )
        {
            i1 = mibp->set( mvp - mibp->mvp, tabix );
            if( i1 < 0 )
            {
                outp = soutp;    /* value field */
            }
        }
        else
        {
            i1 = 0;
        }
		
        if( i1 < 0 )
        {
            ERR_VGO( "MIB set() failed", i1, err_val );
err_val:
            /* Version 2/3 doesn't set errcode for noSuchName */
            if( !version || i1 != noSuchName )
            {
                errcode = i1;   /* Saved from before goto! */
                errix = vindex;
            }
            snmpRWriteVal( &outp, 0, version ? ( nflag ? 0x82 : 0x81 ) : Null, 0 );
            snmpRWriteVal( &outp, inp, Identifier, len2 );  /* Input ID */
            mvp = 0;
        }
        else
        {
         
            if( tabix < 0 ) /* 标量 */
            {
                /*
                 * 将OID及OID长度写入应答buffer中。
                 * 应答消息buffer的内容变为:[...][变量OID类型][变量OID长度][变量OID][变量值类型][变量值长度][变量值内容]
                 */
                snmpRWriteVal( &outp, mvp->oid.name, Identifier, mvp->oid.nlen );
            }
            else /* 表格变量 */
            {
                chkp = outp;

		/*
		 * 将索引值编码,并写入应答buffer中                                                        
		 * 应答消息buffer的内容变为:[...][索引值][变量值类型][变量值长度][变量值内容]
		 */
                snmpEncodeIndex( &outp, mibp, mtp, tabix, mvp );

		/*
		 * 将应答变量的OID及OID长度写入应答buffer中
		 * 应答消息buffer的内容变为:[...][变量OID类型][变量OID长度][变量OID][索引值][变量值类型][变量值长度][变量值内容] 
		 */
                snmpRWriteVal( &outp, mvp->oid.name, Identifier, mvp->oid.nlen );
				
		/*
		 * 移动指针到OID长度处 
		 */
                snmpReadLength( ( const u8_t** )&outp, Identifier ); 

		/*
                 * 修改OID长度，新的长度 = OID长度 + 索引值长度 
	         * 应答消息buffer的内容变为:[...][变量OID类型][变量OID长度(新)][变量OID][索引值][变量值类型][变量值长度][变量值内容]
	         */
                snmpRWriteLength( &outp, Identifier, chkp - outp );
            }
        }


		/*
		 * 写入该应答变量的类型及其长度。
		 * 应答消息buffer的内容变为:[...][名值对x编码类型][名值对x总长度][变量OID类型][变量OID长度(新)][变量OID][索引值][变量值类型][变量值长度][变量值内容]
                 * 自此，一个名值对的内容写入完毕，如果存在多个名值对,则
                 *  应答消息buffer的内容简化为：[...]<名值对1><名值对2><名值对3><名值对x...>
                 */
		snmpRWriteLength( &outp, Sequence, soutp - outp );
		
        /* 前一个变量 */
        vindex--;
    }
    /* Use requested var-binds for bad v1 packet */
    if( version == 0 && errcode )
    {
        /* Point to start of first var-bind */
        i1 = ibpend - varbinds[0];
        outp = *obp - i1;
        memcpy( outp, varbinds[0], i1 );
    }

    /*
     * 写入名值对编码类型，以及所有名值对总长度。
     * 应答消息buffer的内容变为：[...][名值对编码类型][所有名值对总长度]<名值对1><名值对2><名值对3><名值对x...>
     */
    snmpRWriteLength( &outp, Sequence, *obp - outp ); /* Length of varbinds */
	
    if( errcode == 0 )
    {
        errix = 0;
    }
    else if( version == 0 )
    {
        switch( errcode )
        {
            case wrongValue:
            case wrongEncoding:
            case wrongType:
            case wrongLength:
            case inconsistentValue:
                errcode = badValue;
                break;
            case noAccess:
            case notWritable:
            case noCreation:
            case inconsistentName:
                errcode = noSuchName;
                break;
            case resourceUnavailable:
            case commitFailed:
            case undoFailed:
                errcode = genErr;
                break;
            case authorizationError:
                errcode = noSuchName;
                break;
            default:
                break;
        }
    }

    /*
     * 写入名值对前面的字段,完成一个snmp应答报文。
     * 应答消息buffer的内容变为:[编码类型][消息总长度][Version][Community][PDU type][Request Id][Error status][Error index][名值对编码类型][所有名值对总长度]<名值对1><名值对2><名值对3><名值对x...>
     */

    /* Error index */ 
    snmpRWriteInt( &outp, errix, Integer, sizeof( errix ) );  
    /* Error status */
    snmpRWriteInt( &outp, errcode, Integer, sizeof( errcode ) );
    /* Request Id */
    snmpRWriteInt( &outp, reqid, Integer, sizeof( reqid ) );
    /* PDU type */
    snmpRWriteLength( &outp, GetResponse, *obp - outp );
    /* Community */
    snmpRWriteVal( &outp, contextName, String, slength( contextName ) );
    /* Version */
    snmpRWriteInt( &outp, version, Integer, sizeof( version ) ); 

    /* 编码类型、消息总长度 */
    snmpRWriteLength( &outp, Sequence, *obp - outp );


    /* snmp应答报文总长度 */
    i1 = *obp - outp;

    *obp = outp;

    /* 返回snmp应答报文总长度 */
    return i1;
}


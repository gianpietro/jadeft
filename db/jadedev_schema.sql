--
-- PostgreSQL database dump
--

-- Dumped from database version 13.4
-- Dumped by pg_dump version 13.4

SET statement_timeout = 0;
SET lock_timeout = 0;
SET idle_in_transaction_session_timeout = 0;
SET client_encoding = 'UTF8';
SET standard_conforming_strings = on;
SELECT pg_catalog.set_config('search_path', '', false);
SET check_function_bodies = false;
SET xmloption = content;
SET client_min_messages = warning;
SET row_security = off;

SET default_tablespace = '';

SET default_table_access_method = heap;

--
-- Name: category_type; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.category_type (
    id integer NOT NULL,
    category character varying(50) NOT NULL
);


ALTER TABLE public.category_type OWNER TO gianpietro;

--
-- Name: category_type_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.category_type_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.category_type_id_seq OWNER TO gianpietro;

--
-- Name: category_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.category_type_id_seq OWNED BY public.category_type.id;


--
-- Name: document_type; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.document_type (
    type_id integer NOT NULL,
    description character varying(100)
);


ALTER TABLE public.document_type OWNER TO gianpietro;

--
-- Name: document_type_type_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.document_type_type_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.document_type_type_id_seq OWNER TO gianpietro;

--
-- Name: document_type_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.document_type_type_id_seq OWNED BY public.document_type.type_id;


--
-- Name: documents; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.documents (
    document_id integer NOT NULL,
    parent_id integer NOT NULL,
    file_name character varying(30) NOT NULL,
    oid_value integer NOT NULL,
    type_id integer,
    document_ref character varying(50),
    title character varying(100),
    description character varying(150),
    start_date integer,
    end_date integer,
    catalog character varying(30) NOT NULL
);


ALTER TABLE public.documents OWNER TO gianpietro;

--
-- Name: documents_document_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.documents_document_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.documents_document_id_seq OWNER TO gianpietro;

--
-- Name: documents_document_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.documents_document_id_seq OWNED BY public.documents.document_id;


--
-- Name: payment_period; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.payment_period (
    payment_period_id integer NOT NULL,
    period character varying(30)
);


ALTER TABLE public.payment_period OWNER TO gianpietro;

--
-- Name: payment_period_payment_period_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.payment_period_payment_period_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.payment_period_payment_period_id_seq OWNER TO gianpietro;

--
-- Name: payment_period_payment_period_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.payment_period_payment_period_id_seq OWNED BY public.payment_period.payment_period_id;


--
-- Name: property; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.property (
    property_id integer NOT NULL,
    active_ind integer NOT NULL,
    post_code character varying(10) NOT NULL,
    address character varying(30),
    city character varying(30)
);


ALTER TABLE public.property OWNER TO gianpietro;

--
-- Name: property_property_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.property_property_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.property_property_id_seq OWNER TO gianpietro;

--
-- Name: property_property_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.property_property_id_seq OWNED BY public.property.property_id;


--
-- Name: provider; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.provider (
    provider_id integer NOT NULL,
    active_ind integer NOT NULL,
    provider_name character varying(30) NOT NULL
);


ALTER TABLE public.provider OWNER TO gianpietro;

--
-- Name: provider_account; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.provider_account (
    provider_acct_id integer NOT NULL,
    active_ind integer NOT NULL,
    provider_id integer,
    provider_acct_no character varying(30),
    sort_code integer,
    preference character varying(30),
    provider_type_id integer
);


ALTER TABLE public.provider_account OWNER TO gianpietro;

--
-- Name: provider_account_provider_acct_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.provider_account_provider_acct_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.provider_account_provider_acct_id_seq OWNER TO gianpietro;

--
-- Name: provider_account_provider_acct_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.provider_account_provider_acct_id_seq OWNED BY public.provider_account.provider_acct_id;


--
-- Name: provider_provider_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.provider_provider_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.provider_provider_id_seq OWNER TO gianpietro;

--
-- Name: provider_provider_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.provider_provider_id_seq OWNED BY public.provider.provider_id;


--
-- Name: provider_type; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.provider_type (
    provider_type_id integer NOT NULL,
    description character varying(30)
);


ALTER TABLE public.provider_type OWNER TO gianpietro;

--
-- Name: provider_type_provider_type_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.provider_type_provider_type_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.provider_type_provider_type_id_seq OWNER TO gianpietro;

--
-- Name: provider_type_provider_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.provider_type_provider_type_id_seq OWNED BY public.provider_type.provider_type_id;


--
-- Name: statement; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.statement (
    statement_id integer NOT NULL,
    date integer,
    type character(4),
    description character varying(150),
    value numeric(9,2),
    account character varying(50),
    alias character varying(50)
);


ALTER TABLE public.statement OWNER TO gianpietro;

--
-- Name: statement_link; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.statement_link (
    id integer NOT NULL,
    alias character varying(50) NOT NULL,
    category_id integer,
    category character varying(50)
);


ALTER TABLE public.statement_link OWNER TO gianpietro;

--
-- Name: statement_link_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.statement_link_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.statement_link_id_seq OWNER TO gianpietro;

--
-- Name: statement_link_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.statement_link_id_seq OWNED BY public.statement_link.id;


--
-- Name: statement_statement_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.statement_statement_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.statement_statement_id_seq OWNER TO gianpietro;

--
-- Name: statement_statement_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.statement_statement_id_seq OWNED BY public.statement.statement_id;


--
-- Name: supplier; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.supplier (
    supplier_id integer NOT NULL,
    active_ind integer NOT NULL,
    supplier_name character varying(30) NOT NULL
);


ALTER TABLE public.supplier OWNER TO gianpietro;

--
-- Name: supplier_account; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.supplier_account (
    supplier_acct_id integer NOT NULL,
    active_ind integer NOT NULL,
    supplier_acct_ref character varying(30),
    supplier_id integer,
    property_id integer,
    supplier_type_id integer,
    start_date integer,
    end_date integer,
    payment_period_id integer,
    amount numeric(9,2),
    comment character varying(30),
    alias character(10),
    provider_acct_id integer
);


ALTER TABLE public.supplier_account OWNER TO gianpietro;

--
-- Name: supplier_account_supplier_acct_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.supplier_account_supplier_acct_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.supplier_account_supplier_acct_id_seq OWNER TO gianpietro;

--
-- Name: supplier_account_supplier_acct_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.supplier_account_supplier_acct_id_seq OWNED BY public.supplier_account.supplier_acct_id;


--
-- Name: supplier_invoice; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.supplier_invoice (
    supplier_invoice_id integer NOT NULL,
    invoice_no character varying(30) NOT NULL,
    start_date integer,
    end_date integer,
    supplier_acct_id integer,
    description character varying(30),
    amount real
);


ALTER TABLE public.supplier_invoice OWNER TO gianpietro;

--
-- Name: supplier_invoice_supplier_invoice_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.supplier_invoice_supplier_invoice_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.supplier_invoice_supplier_invoice_id_seq OWNER TO gianpietro;

--
-- Name: supplier_invoice_supplier_invoice_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.supplier_invoice_supplier_invoice_id_seq OWNED BY public.supplier_invoice.supplier_invoice_id;


--
-- Name: supplier_supplier_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.supplier_supplier_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.supplier_supplier_id_seq OWNER TO gianpietro;

--
-- Name: supplier_supplier_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.supplier_supplier_id_seq OWNED BY public.supplier.supplier_id;


--
-- Name: supplier_type; Type: TABLE; Schema: public; Owner: gianpietro
--

CREATE TABLE public.supplier_type (
    supplier_type_id integer NOT NULL,
    description character varying(30)
);


ALTER TABLE public.supplier_type OWNER TO gianpietro;

--
-- Name: supplier_type_supplier_type_id_seq; Type: SEQUENCE; Schema: public; Owner: gianpietro
--

CREATE SEQUENCE public.supplier_type_supplier_type_id_seq
    AS integer
    START WITH 1
    INCREMENT BY 1
    NO MINVALUE
    NO MAXVALUE
    CACHE 1;


ALTER TABLE public.supplier_type_supplier_type_id_seq OWNER TO gianpietro;

--
-- Name: supplier_type_supplier_type_id_seq; Type: SEQUENCE OWNED BY; Schema: public; Owner: gianpietro
--

ALTER SEQUENCE public.supplier_type_supplier_type_id_seq OWNED BY public.supplier_type.supplier_type_id;


--
-- Name: category_type id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.category_type ALTER COLUMN id SET DEFAULT nextval('public.category_type_id_seq'::regclass);


--
-- Name: document_type type_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.document_type ALTER COLUMN type_id SET DEFAULT nextval('public.document_type_type_id_seq'::regclass);


--
-- Name: documents document_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.documents ALTER COLUMN document_id SET DEFAULT nextval('public.documents_document_id_seq'::regclass);


--
-- Name: payment_period payment_period_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.payment_period ALTER COLUMN payment_period_id SET DEFAULT nextval('public.payment_period_payment_period_id_seq'::regclass);


--
-- Name: property property_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.property ALTER COLUMN property_id SET DEFAULT nextval('public.property_property_id_seq'::regclass);


--
-- Name: provider provider_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider ALTER COLUMN provider_id SET DEFAULT nextval('public.provider_provider_id_seq'::regclass);


--
-- Name: provider_account provider_acct_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider_account ALTER COLUMN provider_acct_id SET DEFAULT nextval('public.provider_account_provider_acct_id_seq'::regclass);


--
-- Name: provider_type provider_type_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider_type ALTER COLUMN provider_type_id SET DEFAULT nextval('public.provider_type_provider_type_id_seq'::regclass);


--
-- Name: statement statement_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.statement ALTER COLUMN statement_id SET DEFAULT nextval('public.statement_statement_id_seq'::regclass);


--
-- Name: statement_link id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.statement_link ALTER COLUMN id SET DEFAULT nextval('public.statement_link_id_seq'::regclass);


--
-- Name: supplier supplier_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier ALTER COLUMN supplier_id SET DEFAULT nextval('public.supplier_supplier_id_seq'::regclass);


--
-- Name: supplier_account supplier_acct_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account ALTER COLUMN supplier_acct_id SET DEFAULT nextval('public.supplier_account_supplier_acct_id_seq'::regclass);


--
-- Name: supplier_invoice supplier_invoice_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_invoice ALTER COLUMN supplier_invoice_id SET DEFAULT nextval('public.supplier_invoice_supplier_invoice_id_seq'::regclass);


--
-- Name: supplier_type supplier_type_id; Type: DEFAULT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_type ALTER COLUMN supplier_type_id SET DEFAULT nextval('public.supplier_type_supplier_type_id_seq'::regclass);


--
-- Name: category_type category_type_category_key; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.category_type
    ADD CONSTRAINT category_type_category_key UNIQUE (category);


--
-- Name: category_type category_type_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.category_type
    ADD CONSTRAINT category_type_pkey PRIMARY KEY (id);


--
-- Name: document_type document_type_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.document_type
    ADD CONSTRAINT document_type_pkey PRIMARY KEY (type_id);


--
-- Name: documents documents_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.documents
    ADD CONSTRAINT documents_pkey PRIMARY KEY (document_id);


--
-- Name: payment_period payment_period_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.payment_period
    ADD CONSTRAINT payment_period_pkey PRIMARY KEY (payment_period_id);


--
-- Name: property property_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.property
    ADD CONSTRAINT property_pkey PRIMARY KEY (property_id);


--
-- Name: provider_account provider_account_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider_account
    ADD CONSTRAINT provider_account_pkey PRIMARY KEY (provider_acct_id);


--
-- Name: provider_account provider_account_provider_acct_no_key; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider_account
    ADD CONSTRAINT provider_account_provider_acct_no_key UNIQUE (provider_acct_no);


--
-- Name: provider provider_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider
    ADD CONSTRAINT provider_pkey PRIMARY KEY (provider_id);


--
-- Name: provider_type provider_type_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider_type
    ADD CONSTRAINT provider_type_pkey PRIMARY KEY (provider_type_id);


--
-- Name: statement_link statement_link_alias_key; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.statement_link
    ADD CONSTRAINT statement_link_alias_key UNIQUE (alias);


--
-- Name: statement_link statement_link_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.statement_link
    ADD CONSTRAINT statement_link_pkey PRIMARY KEY (id);


--
-- Name: statement statement_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.statement
    ADD CONSTRAINT statement_pkey PRIMARY KEY (statement_id);


--
-- Name: supplier_account supplier_account_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account
    ADD CONSTRAINT supplier_account_pkey PRIMARY KEY (supplier_acct_id);


--
-- Name: supplier_account supplier_account_supplier_acct_ref_key; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account
    ADD CONSTRAINT supplier_account_supplier_acct_ref_key UNIQUE (supplier_acct_ref);


--
-- Name: supplier_invoice supplier_invoice_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_invoice
    ADD CONSTRAINT supplier_invoice_pkey PRIMARY KEY (supplier_invoice_id);


--
-- Name: supplier supplier_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier
    ADD CONSTRAINT supplier_pkey PRIMARY KEY (supplier_id);


--
-- Name: supplier_type supplier_type_pkey; Type: CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_type
    ADD CONSTRAINT supplier_type_pkey PRIMARY KEY (supplier_type_id);


--
-- Name: documents documents_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.documents
    ADD CONSTRAINT documents_type_id_fkey FOREIGN KEY (type_id) REFERENCES public.document_type(type_id) ON DELETE CASCADE;


--
-- Name: provider_account provider_account_provider_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider_account
    ADD CONSTRAINT provider_account_provider_id_fkey FOREIGN KEY (provider_id) REFERENCES public.provider(provider_id);


--
-- Name: provider_account provider_account_provider_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.provider_account
    ADD CONSTRAINT provider_account_provider_type_id_fkey FOREIGN KEY (provider_type_id) REFERENCES public.provider_type(provider_type_id);


--
-- Name: statement_link statement_link_category_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.statement_link
    ADD CONSTRAINT statement_link_category_id_fkey FOREIGN KEY (category_id) REFERENCES public.category_type(id);


--
-- Name: supplier_account supplier_account_payment_period_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account
    ADD CONSTRAINT supplier_account_payment_period_id_fkey FOREIGN KEY (payment_period_id) REFERENCES public.payment_period(payment_period_id);


--
-- Name: supplier_account supplier_account_property_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account
    ADD CONSTRAINT supplier_account_property_id_fkey FOREIGN KEY (property_id) REFERENCES public.property(property_id);


--
-- Name: supplier_account supplier_account_provider_acct_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account
    ADD CONSTRAINT supplier_account_provider_acct_id_fkey FOREIGN KEY (provider_acct_id) REFERENCES public.provider_account(provider_acct_id) ON DELETE CASCADE;


--
-- Name: supplier_account supplier_account_supplier_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account
    ADD CONSTRAINT supplier_account_supplier_id_fkey FOREIGN KEY (supplier_id) REFERENCES public.supplier(supplier_id);


--
-- Name: supplier_account supplier_account_supplier_type_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_account
    ADD CONSTRAINT supplier_account_supplier_type_id_fkey FOREIGN KEY (supplier_type_id) REFERENCES public.supplier_type(supplier_type_id);


--
-- Name: supplier_invoice supplier_invoice_supplier_acct_id_fkey; Type: FK CONSTRAINT; Schema: public; Owner: gianpietro
--

ALTER TABLE ONLY public.supplier_invoice
    ADD CONSTRAINT supplier_invoice_supplier_acct_id_fkey FOREIGN KEY (supplier_acct_id) REFERENCES public.supplier_account(supplier_acct_id) ON DELETE CASCADE;


--
-- PostgreSQL database dump complete
--


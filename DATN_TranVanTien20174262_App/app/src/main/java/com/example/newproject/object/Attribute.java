package com.example.newproject.object;

public class Attribute {
    private String entity_type;
    private String entity_id;
    private String attribute_type;
    private String attribute_key;
    private boolean logged;
    private String value;
    private long last_update_ts;
    private String value_type;

    public String getEntity_type() {
        return entity_type;
    }

    public void setEntity_type(String entity_type) {
        this.entity_type = entity_type;
    }

    public String getEntity_id() {
        return entity_id;
    }

    public void setEntity_id(String entity_id) {
        this.entity_id = entity_id;
    }

    public String getAttribute_type() {
        return attribute_type;
    }

    public void setAttribute_type(String attribute_type) {
        this.attribute_type = attribute_type;
    }

    public String getAttribute_key() {
        return attribute_key;
    }

    public void setAttribute_key(String attribute_key) {
        this.attribute_key = attribute_key;
    }

    public boolean isLogged() {
        return logged;
    }

    public void setLogged(boolean logged) {
        this.logged = logged;
    }

    public String getValue() {
        return value;
    }

    public void setValue(String value) {
        this.value = value;
    }

    public long getLast_update_ts() {
        return last_update_ts;
    }

    public void setLast_update_ts(long last_update_ts) {
        this.last_update_ts = last_update_ts;
    }

    public String getValue_type() {
        return value_type;
    }

    public void setValue_type(String value_type) {
        this.value_type = value_type;
    }
}
